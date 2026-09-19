#include "GameState.h"

#include <cstring>

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;
using namespace VEngine::Network;

namespace
{
    float Hash01(uint32_t value)
    {
        value ^= value << 13;
        value ^= value >> 17;
        value ^= value << 5;
        return static_cast<float>(value % 10000) / 10000.0f;
    }

    float Lerp(float a, float b, float t)
    {
        return a + ((b - a) * t);
    }
}

GameState::ThreadPool::~ThreadPool()
{
    Stop();
}

void GameState::ThreadPool::Start(uint32_t threadCount)
{
    Stop();

    mThreadCount = threadCount;
    mRunning = true;

    for (uint32_t i = 0; i < mThreadCount; ++i)
    {
        mWorkers.emplace_back([this]()
            {
                WorkerLoop();
            });
    }
}

void GameState::ThreadPool::Stop()
{
    {
        std::lock_guard<std::mutex> lock(mQueueMutex);
        mRunning = false;
    }

    mQueueCondition.notify_all();

    for (std::thread& worker : mWorkers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    mWorkers.clear();
    mThreadCount = 0;
}

void GameState::ThreadPool::AddTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> waitLock(mWaitMutex);
        ++mActiveTasks;
    }

    {
        std::lock_guard<std::mutex> queueLock(mQueueMutex);
        mTasks.push(std::move(task));
    }

    mQueueCondition.notify_one();
}

void GameState::ThreadPool::Wait()
{
    std::unique_lock<std::mutex> lock(mWaitMutex);

    mWaitCondition.wait(lock, [this]()
        {
            return mActiveTasks == 0;
        });
}

void GameState::ThreadPool::WorkerLoop()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mQueueMutex);

            mQueueCondition.wait(lock, [this]()
                {
                    return !mRunning || !mTasks.empty();
                });

            if (!mRunning && mTasks.empty())
            {
                return;
            }

            task = std::move(mTasks.front());
            mTasks.pop();
        }

        task();

        {
            std::lock_guard<std::mutex> lock(mWaitMutex);
            --mActiveTasks;
        }

        mWaitCondition.notify_one();
    }
}

void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 7.0f, -10.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 2.0f });

    const uint32_t hardwareThreads = std::thread::hardware_concurrency();
    const uint32_t workerCount = hardwareThreads > 1 ? hardwareThreads - 1 : 1;

    mThreadPool.Start(workerCount);

    ResetGame();
}

void GameState::Terminate()
{
    StopNetwork();

    mThreadPool.Stop();
    mAsteroids.clear();
}

void GameState::ResetGame()
{
    mPlayers[0] = PlayerState{};
    mPlayers[1] = PlayerState{};

    // Player 1 starts on the left.
    mPlayers[0].position =
    {
        -1.0f,
        0.25f,
        -3.0f
    };

    // Player 2 starts on the right.
    mPlayers[1].position =
    {
        1.0f,
        0.25f,
        -3.0f
    };

    mPlayers[0].alive = true;
    mPlayers[1].alive = true;

    mPlayers[0].survivalTime = 0.0f;
    mPlayers[1].survivalTime = 0.0f;

    mRemoteMovement = 0.0f;

    mMatchOver = false;
    mWinner = 0;

    mStateBroadcastTimer = 0.0f;
    mAsteroidBroadcastTimer = 0.0f;

    mAsteroids.clear();

    mAsteroids.resize(
        static_cast<size_t>(mAsteroidCount));

    for (size_t i = 0;
        i < mAsteroids.size();
        ++i)
    {
        ResetAsteroid(i);
    }
}

void GameState::Update(float deltaTime)
{
    UpdateNetwork(deltaTime);

    InputSystem* input = InputSystem::Get();

    UpdateCamera(deltaTime);

    // HOST controls restarting the match.
    if (mNetworkMode == NetworkMode::Host &&
        mNetworkConnected &&
        input->IsKeyPressed(KeyCode::R))
    {
        ResetGame();

        mMatchStarted = true;

        RestartPacket packet;

        mServer->SendMsg(
            reinterpret_cast<const char*>(&packet),
            sizeof(packet));

        SendGameState();
        SendAsteroidSnapshot();
    }

    // Nothing should simulate until both players
    // are connected.
    if (!mNetworkConnected ||
        !mMatchStarted)
    {
        return;
    }

    // ===============================
    // HOST
    // ===============================

    if (mNetworkMode == NetworkMode::Host)
    {
        if (!mMatchOver)
        {
            UpdatePlayer(deltaTime);

            // Count survival time only while alive.
            for (PlayerState& player : mPlayers)
            {
                if (player.alive)
                {
                    player.survivalTime += deltaTime;
                }
            }

            // ONLY THE HOST updates the real asteroids.
            if (mUseParallelUpdate)
            {
                UpdateAsteroidsParallel(deltaTime);
            }
            else
            {
                UpdateAsteroidRange(
                    0,
                    mAsteroids.size(),
                    deltaTime);
            }

            // Collision is server authoritative.
            CheckPlayerCollisions();
        }

        // Send player/game state frequently.
        mStateBroadcastTimer -= deltaTime;

        if (mStateBroadcastTimer <= 0.0f)
        {
            SendGameState();

            mStateBroadcastTimer +=
                mStateBroadcastRate;
        }

        // Send asteroid corrections less frequently.
        mAsteroidBroadcastTimer -= deltaTime;

        if (mAsteroidBroadcastTimer <= 0.0f)
        {
            SendAsteroidSnapshot();

            mAsteroidBroadcastTimer +=
                mAsteroidBroadcastRate;
        }
    }

    // ===============================
    // CLIENT
    // ===============================

    else if (mNetworkMode == NetworkMode::Client)
    {
        if (!mMatchOver)
        {
            UpdatePlayer(deltaTime);

            // Client only predicts asteroid movement
            // between server updates.
            for (Asteroid& asteroid : mAsteroids)
            {
                asteroid.position.z +=
                    asteroid.velocity.z * deltaTime;
            }
        }
    }
}

void GameState::UpdatePlayer(float deltaTime)
{
    InputSystem* input = InputSystem::Get();

    float movement = 0.0f;

    if (input->IsKeyDown(KeyCode::A) ||
        input->IsKeyDown(KeyCode::LEFT))
    {
        movement -= 1.0f;
    }

    if (input->IsKeyDown(KeyCode::D) ||
        input->IsKeyDown(KeyCode::RIGHT))
    {
        movement += 1.0f;
    }

    // ===============================
    // PLAYER 1 / HOST
    // ===============================

    if (mNetworkMode == NetworkMode::Host)
    {
        if (mPlayers[0].alive)
        {
            ApplyMovement(
                mPlayers[0],
                movement,
                deltaTime);
        }

        // Player 2 movement comes from network.
        if (mPlayers[1].alive)
        {
            ApplyMovement(
                mPlayers[1],
                mRemoteMovement,
                deltaTime);
        }
    }

    // ===============================
    // PLAYER 2 / CLIENT
    // ===============================

    else if (mNetworkMode == NetworkMode::Client)
    {
        // Local prediction.
        if (mPlayers[1].alive)
        {
            ApplyMovement(
                mPlayers[1],
                movement,
                deltaTime);
        }

        PlayerInputPacket packet;

        packet.playerID = 2;

        if (mPlayers[1].alive)
        {
            packet.movement = movement;
        }
        else
        {
            packet.movement = 0.0f;
        }

        mClient->SendMsg(
            reinterpret_cast<const char*>(&packet),
            sizeof(packet));
    }
}

void GameState::UpdateAsteroidsParallel(float deltaTime)
{
    const size_t asteroidCount = mAsteroids.size();
    const uint32_t workerCount = mThreadPool.GetThreadCount();

    if (asteroidCount == 0 || workerCount == 0)
    {
        return;
    }

    const size_t chunkSize = (asteroidCount + workerCount - 1) / workerCount;

    for (size_t start = 0; start < asteroidCount; start += chunkSize)
    {
        const size_t end = std::min(start + chunkSize, asteroidCount);

        mThreadPool.AddTask([this, start, end, deltaTime]()
            {
                UpdateAsteroidRange(start, end, deltaTime);
            });
    }

    mThreadPool.Wait();
}

void GameState::UpdateAsteroidRange(size_t startIndex, size_t endIndex, float deltaTime)
{
    for (size_t i = startIndex; i < endIndex; ++i)
    {
        Asteroid& asteroid = mAsteroids[i];

        asteroid.position.x += asteroid.velocity.x * deltaTime;
        asteroid.position.y += asteroid.velocity.y * deltaTime;
        asteroid.position.z += asteroid.velocity.z * deltaTime;

        if (asteroid.position.z < mDeathZ)
        {
            ++asteroid.resetCount;
            ResetAsteroid(i);
        }
    }
}

void GameState::ResetAsteroid(size_t index)
{
    Asteroid& asteroid = mAsteroids[index];

    const uint32_t seed = static_cast<uint32_t>(index * 9781 + asteroid.resetCount * 6271);

    const float x01 = Hash01(seed + 11);
    const float z01 = Hash01(seed + 23);
    const float speed01 = Hash01(seed + 37);
    const float radius01 = Hash01(seed + 49);

    asteroid.position.x = Lerp(-mArenaHalfWidth, mArenaHalfWidth, x01);
    asteroid.position.y = 0.25f;
    asteroid.position.z = Lerp(2.0f, mSpawnZ, z01);

    asteroid.velocity.x = 0.0f;
    asteroid.velocity.y = 0.0f;
    asteroid.velocity.z = -Lerp(1.5f, 4.5f, speed01);

    asteroid.radius = Lerp(0.08f, 0.22f, radius01);
}

void GameState::ApplyMovement(PlayerState& player, float movement, float deltaTime)
{
    player.position.x += movement * mPlayerSpeed * deltaTime;

    if (player.position.x < -mArenaHalfWidth)
    {
        player.position.x = -mArenaHalfWidth;
    }

    if (player.position.x > mArenaHalfWidth)
    {
        player.position.x = mArenaHalfWidth;
    }
}

void GameState::CheckPlayerCollisions()
{
    for (const Asteroid& asteroid : mAsteroids)
    {
        for (PlayerState& player : mPlayers)
        {
            if (!player.alive)
            {
                continue;
            }

            if (CheckCollision(
                asteroid,
                player))
            {
                player.alive = false;
            }
        }
    }

    // Match ends after BOTH players have died.
    if (!mPlayers[0].alive &&
        !mPlayers[1].alive &&
        !mMatchOver)
    {
        mMatchOver = true;

        const float player1Time =
            mPlayers[0].survivalTime;

        const float player2Time =
            mPlayers[1].survivalTime;

        const float difference =
            player1Time - player2Time;

        if (difference > 0.001f)
        {
            mWinner = 1;
        }
        else if (difference < -0.001f)
        {
            mWinner = 2;
        }
        else
        {
            mWinner = 0;
        }

        SendGameState();
    }
}

bool GameState::CheckCollision(const Asteroid& asteroid, const PlayerState& player) const
{
    const float dx = asteroid.position.x - player.position.x;

    const float dz = asteroid.position.z - player.position.z;

    const float distanceSquared = (dx * dx) + (dz * dz);

    const float radiusSum = asteroid.radius + player.radius;

    return distanceSquared <= radiusSum * radiusSum;
}

void GameState::SendGameState()
{
    if (!mServer ||
        !mNetworkConnected)
    {
        return;
    }

    GameStatePacket packet;

    for (int i = 0; i < 2; ++i)
    {
        packet.players[i].x = mPlayers[i].position.x;

        packet.players[i].y = mPlayers[i].position.y;

        packet.players[i].z = mPlayers[i].position.z;

        packet.players[i].survivalTime = mPlayers[i].survivalTime;

        packet.players[i].alive = mPlayers[i].alive ? 1 : 0;
    }

    packet.matchStarted = mMatchStarted ? 1 : 0;

    packet.matchOver = mMatchOver ? 1 : 0;

    packet.winner = mWinner;

    mServer->SendMsg(reinterpret_cast<const char*>(&packet), sizeof(packet));
}

void GameState::SendAsteroidSnapshot()
{
    if (!mServer ||
        !mNetworkConnected)
    {
        return;
    }

    for (size_t start = 0;
        start < mAsteroids.size();
        start += ASTEROIDS_PER_PACKET)
    {
        AsteroidChunkPacket packet;

        packet.startIndex =
            static_cast<uint32_t>(start);

        packet.count =
            static_cast<uint32_t>(
                std::min<size_t>(
                    ASTEROIDS_PER_PACKET,
                    mAsteroids.size() - start));

        for (uint32_t i = 0;
            i < packet.count;
            ++i)
        {
            const Asteroid& asteroid =
                mAsteroids[start + i];

            NetworkAsteroidState&
                networkAsteroid =
                packet.asteroids[i];

            networkAsteroid.x =
                asteroid.position.x;

            networkAsteroid.y =
                asteroid.position.y;

            networkAsteroid.z =
                asteroid.position.z;

            networkAsteroid.velocityZ =
                asteroid.velocity.z;

            networkAsteroid.radius =
                asteroid.radius;

            networkAsteroid.resetCount =
                asteroid.resetCount;
        }

        mServer->SendMsg(
            reinterpret_cast<const char*>(&packet),
            sizeof(packet));
    }
}

void GameState::ProcessServerPacket(const char* data, int dataLength)
{
    if (data == nullptr ||
        dataLength <
        static_cast<int>(sizeof(AsteroidPacketType)))
    {
        return;
    }

    AsteroidPacketType packetType;

    std::memcpy(
        &packetType,
        data,
        sizeof(packetType));

    switch (packetType)
    {
        // =====================================
        // CLIENT JOINS THE HOST
        // =====================================
    case AsteroidPacketType::Join:
    {
        mNetworkConnected = true;

        mNetworkStatus =
            "Player 2 connected.";

        // Tell the client that it is Player 2.
        WelcomePacket welcomePacket;

        welcomePacket.playerID = 2;

        welcomePacket.asteroidCount =
            static_cast<uint32_t>(
                mAsteroids.size());

        mServer->SendMsg(
            reinterpret_cast<const char*>(
                &welcomePacket),
            sizeof(welcomePacket));

        // Start one shared match.
        ResetGame();

        mMatchStarted = true;
        mMatchOver = false;
        mWinner = 0;

        // Send the starting world state
        // immediately to Player 2.
        SendGameState();
        SendAsteroidSnapshot();

        break;
    }

    // =====================================
    // PLAYER 2 INPUT
    // =====================================
    case AsteroidPacketType::PlayerInput:
    {
        if (dataLength <
            static_cast<int>(
                sizeof(PlayerInputPacket)))
        {
            break;
        }

        PlayerInputPacket inputPacket;

        std::memcpy(
            &inputPacket,
            data,
            sizeof(inputPacket));

        // Only Player 2 should send
        // movement to the host.
        if (inputPacket.playerID == 2)
        {
            mRemoteMovement =
                inputPacket.movement;

            // Prevent invalid movement values.
            if (mRemoteMovement < -1.0f)
            {
                mRemoteMovement = -1.0f;
            }

            if (mRemoteMovement > 1.0f)
            {
                mRemoteMovement = 1.0f;
            }
        }

        break;
    }

    default:
    {
        break;
    }
    }
}

void GameState::ProcessClientPacket(const char* data, int dataLength)
{
    if (data == nullptr ||
        dataLength <
        static_cast<int>(sizeof(AsteroidPacketType)))
    {
        return;
    }

    AsteroidPacketType packetType;

    std::memcpy(
        &packetType,
        data,
        sizeof(packetType));

    switch (packetType)
    {
        // =====================================
        // WELCOME
        // =====================================

    case AsteroidPacketType::Welcome:
    {
        if (dataLength <
            static_cast<int>(
                sizeof(WelcomePacket)))
        {
            break;
        }

        WelcomePacket packet;

        std::memcpy(
            &packet,
            data,
            sizeof(packet));

        mPlayerID =
            packet.playerID;

        mNetworkConnected = true;

        mNetworkStatus =
            "Connected as Player "
            + std::to_string(mPlayerID);

        // Make sure client has same
        // number of asteroids as server.
        mAsteroids.resize(
            packet.asteroidCount);

        break;
    }

    // =====================================
    // GAME STATE
    // =====================================

    case AsteroidPacketType::GameState:
    {
        if (dataLength <
            static_cast<int>(
                sizeof(GameStatePacket)))
        {
            break;
        }

        GameStatePacket packet;

        std::memcpy(
            &packet,
            data,
            sizeof(packet));

        for (int i = 0; i < 2; ++i)
        {
            mPlayers[i].position =
            {
                packet.players[i].x,
                packet.players[i].y,
                packet.players[i].z
            };

            mPlayers[i].survivalTime =
                packet.players[i].survivalTime;

            mPlayers[i].alive =
                packet.players[i].alive != 0;
        }

        mMatchStarted =
            packet.matchStarted != 0;

        mMatchOver =
            packet.matchOver != 0;

        mWinner =
            packet.winner;

        break;
    }

    // =====================================
    // ASTEROIDS
    // =====================================

    case AsteroidPacketType::AsteroidChunk:
    {
        if (dataLength <
            static_cast<int>(
                sizeof(AsteroidChunkPacket)))
        {
            break;
        }

        AsteroidChunkPacket packet;

        std::memcpy(
            &packet,
            data,
            sizeof(packet));

        for (uint32_t i = 0;
            i < packet.count;
            ++i)
        {
            const size_t index =
                packet.startIndex + i;

            if (index >=
                mAsteroids.size())
            {
                continue;
            }

            Asteroid& asteroid =
                mAsteroids[index];

            const NetworkAsteroidState&
                networkAsteroid =
                packet.asteroids[i];

            asteroid.position =
            {
                networkAsteroid.x,
                networkAsteroid.y,
                networkAsteroid.z
            };

            asteroid.velocity =
            {
                0.0f,
                0.0f,
                networkAsteroid.velocityZ
            };

            asteroid.radius =
                networkAsteroid.radius;

            asteroid.resetCount =
                networkAsteroid.resetCount;
        }

        break;
    }

    // =====================================
    // RESTART
    // =====================================

    case AsteroidPacketType::Restart:
    {
        ResetGame();

        mMatchStarted = true;

        break;
    }

    default:
    {
        break;
    }
    }
}

bool GameState::CheckCollision(const Asteroid& asteroid, const PlayerState& player) const
{
    const float dx = asteroid.position.x - player.position.x;

    const float dz = asteroid.position.z - player.position.z;

    const float distanceSquared = (dx * dx) + (dz * dz);

    const float radiusSum = asteroid.radius + player.radius;

    return distanceSquared <= radiusSum * radiusSum;
}

void GameState::StartHost()
{
    StopNetwork();

    HWND window = GetActiveWindow();

    if (window == nullptr)
    {
        mNetworkStatus = "Could not get VEngine window.";
        return;
    }

    mServer = std::make_unique<Network::Server>(8000);

    mServer->Initialize(window, "");

    if (!mServer->IsInitialized())
    {
        mNetworkStatus =
            "Server failed. WSA Error: "
            + std::to_string(mServer->GetLastError());

        mServer.reset();
        return;
    }

    mNetworkMode = NetworkMode::Host;

    // Host is always Player 1.
    mPlayerID = 1;

    mNetworkConnected = false;

    mNetworkStatus =
        "Host started. Waiting for Player 2...";
}

void GameState::StartClient()
{
    StopNetwork();

    HWND window = GetActiveWindow();

    if (window == nullptr)
    {
        mNetworkStatus = "Could not get VEngine window.";
        return;
    }

    mClient = std::make_unique<Network::Client>(8000);

    mClient->Initialize(
        window,
        mServerAddress);

    if (!mClient->IsInitialized())
    {
        mNetworkStatus =
            "Client failed. WSA Error: "
            + std::to_string(mClient->GetLastError());

        mClient.reset();
        return;
    }

    mNetworkMode = NetworkMode::Client;

    mPlayerID = -1;

    mNetworkConnected = false;

    JoinPacket packet;

    mClient->SendMsg(
        reinterpret_cast<const char*>(&packet),
        sizeof(packet));

    mNetworkStatus =
        "Join request sent...";
}

void GameState::StopNetwork()
{
    if (mServer)
    {
        mServer->Terminate();
        mServer.reset();
    }

    if (mClient)
    {
        mClient->Terminate();
        mClient.reset();
    }

    mNetworkMode = NetworkMode::None;
    mPlayerID = -1;
    mNetworkConnected = false;
}

void GameState::UpdateNetwork(float deltaTime)
{
    // =============================
    // HOST / SERVER
    // =============================
    if (mNetworkMode == NetworkMode::Host &&
        mServer)
    {
        mServer->ResetMsg();
        mServer->ReceiveMsg();

        const int dataLength =
            mServer->GetDataLength();

        if (dataLength > 0)
        {
            ProcessServerPacket(
                mServer->GetData(),
                dataLength);
        }
    }

    // =============================
    // CLIENT
    // =============================
    if (mNetworkMode == NetworkMode::Client &&
        mClient)
    {
        mClient->ResetMsg();
        mClient->ReceiveMsg();

        const int dataLength =
            mClient->GetDataLength();

        if (dataLength > 0)
        {
            ProcessClientPacket(
                mClient->GetData(),
                dataLength);
        }
    }

    // =============================
    // SERVER
    // =============================

    if (mNetworkMode == NetworkMode::Host && mServer)
    {
        mServer->ResetMsg();
        mServer->ReceiveMsg();

        const int dataLength = mServer->GetDataLength();

        if (dataLength > 0)
        {
            ProcessServerPacket(mServer->GetData(), dataLength);
        }
    }
}

void GameState::Render()
{
    SimpleDraw::AddGroundPlane(12.0f, Colors::DarkSeaGreen);

    // Arena boundaries
    SimpleDraw::AddLine({ -mArenaHalfWidth, 0.05f, mDeathZ }, { -mArenaHalfWidth, 0.05f, mSpawnZ }, Colors::White);
    SimpleDraw::AddLine({ mArenaHalfWidth, 0.05f, mDeathZ }, { mArenaHalfWidth, 0.05f, mSpawnZ }, Colors::White);
    SimpleDraw::AddLine({ -mArenaHalfWidth, 0.05f, mDeathZ }, { mArenaHalfWidth, 0.05f, mDeathZ }, Colors::White);
    SimpleDraw::AddLine({ -mArenaHalfWidth, 0.05f, mSpawnZ }, { mArenaHalfWidth, 0.05f, mSpawnZ }, Colors::White);

    // Player 1
    Color player1Color = mPlayers[0].alive ? Colors::Cyan:Colors::Red;

    SimpleDraw::AddSphere(
        16,
        16,
        mPlayers[0].radius,
        mPlayers[0].position,
        player1Color);

    // Player 2
    if (mNetworkConnected)
    {
        Color player2Color =
            mPlayers[1].alive
            ? Colors::Blue
            : Colors::Red;

        SimpleDraw::AddSphere(
            16,
            16,
            mPlayers[1].radius,
            mPlayers[1].position,
            player2Color);
    }

    // Asteroids
    for (const Asteroid& asteroid : mAsteroids)
    {
        SimpleDraw::AddSphere(8, 8, asteroid.radius, asteroid.position, Colors::Orange);
    }

    SimpleDraw::Render(mCamera);
}

void GameState::DebugUI()
{
    ImGui::Begin("Parallel Asteroid Dodge", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Controls:");
    ImGui::Text("A / Left Arrow  = Move Left");
    ImGui::Text("D / Right Arrow = Move Right");
    ImGui::Text("R = Restart");

    ImGui::Separator();

    ImGui::Text("Score: %d", mScore.load());
    ImGui::Text("Asteroid Count: %d", static_cast<int>(mAsteroids.size()));
    ImGui::Text("Worker Threads: %u", mThreadPool.GetThreadCount());

    ImGui::Text("SURVIVAL");

    ImGui::Text(
        "Player 1: %.2f seconds%s",
        mPlayers[0].survivalTime,
        mPlayers[0].alive
        ? ""
        : " - ELIMINATED");

    ImGui::Text(
        "Player 2: %.2f seconds%s",
        mPlayers[1].survivalTime,
        mPlayers[1].alive
        ? ""
        : " - ELIMINATED");

    if (mMatchOver)
    {
        ImGui::Separator();

        if (mWinner == 1)
        {
            ImGui::Text("PLAYER 1 WINS!");
        }
        else if (mWinner == 2)
        {
            ImGui::Text("PLAYER 2 WINS!");
        }
        else
        {
            ImGui::Text("TIE!");
        }

        if (mNetworkMode == NetworkMode::Host)
        {
            ImGui::Text(
                "Press R to restart.");
        }
    }

    ImGui::Separator();

    if (ImGui::Button("Restart Game"))
    {
        ResetGame();
    }

    ImGui::Checkbox("Use Parallel Update", &mUseParallelUpdate);

    if (ImGui::SliderInt("Asteroid Count", &mAsteroidCount, 10, 500))
    {
        ResetGame();
    }

    ImGui::Text("MULTIPLAYER");

    ImGui::Separator();

    ImGui::Text(
        "Status: %s",
        mNetworkStatus.c_str());

    if (mNetworkMode == NetworkMode::Host)
    {
        ImGui::Text("Mode: Host");
    }
    else if (mNetworkMode == NetworkMode::Client)
    {
        ImGui::Text("Mode: Client");
    }
    else
    {
        ImGui::Text("Mode: None");
    }

    if (mPlayerID != -1)
    {
        ImGui::Text(
            "Player ID: %d",
            mPlayerID);
    }
    else
    {
        ImGui::Text("Player ID: Waiting...");
    }

    ImGui::InputText(
        "Server IP",
        mServerAddress,
        sizeof(mServerAddress));

    if (ImGui::Button("Start Host"))
    {
        StartHost();
    }

    ImGui::SameLine();

    if (ImGui::Button("Join Host"))
    {
        StartClient();
    }

    ImGui::SameLine();

    if (ImGui::Button("Disconnect"))
    {
        StopNetwork();

        mNetworkStatus =
            "Not connected";
    }

    ImGui::Separator();

    ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
    InputSystem* input = InputSystem::Get();

    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
    const float turnSpeed = 0.5f;

    if (input->IsKeyDown(KeyCode::E))
    {
        mCamera.Rise(moveSpeed * deltaTime);
    }
    else if (input->IsKeyDown(KeyCode::Q))
    {
        mCamera.Rise(-moveSpeed * deltaTime);
    }

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }
}