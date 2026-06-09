#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

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
    mThreadPool.Stop();
    mAsteroids.clear();
}

void GameState::ResetGame()
{
    mScore = 0;
    mGameOver = false;

    mPlayerPosition = { 0.0f, 0.25f, -3.0f };

    mAsteroids.clear();
    mAsteroids.resize(static_cast<size_t>(mAsteroidCount));

    for (size_t i = 0; i < mAsteroids.size(); ++i)
    {
        ResetAsteroid(i);
    }
}

void GameState::Update(float deltaTime)
{
    InputSystem* input = InputSystem::Get();

    if (input->IsKeyPressed(KeyCode::R))
    {
        ResetGame();
    }

    if (input->IsKeyPressed(KeyCode::SPACE))
    {
        mUseParallelUpdate = !mUseParallelUpdate;
    }

    UpdateCamera(deltaTime);

    if (mGameOver)
    {
        return;
    }

    UpdatePlayer(deltaTime);

    if (mUseParallelUpdate)
    {
        UpdateAsteroidsParallel(deltaTime);
    }
    else
    {
        UpdateAsteroidRange(0, mAsteroids.size(), deltaTime);
    }
}

void GameState::UpdatePlayer(float deltaTime)
{
    InputSystem* input = InputSystem::Get();

    float movement = 0.0f;

    if (input->IsKeyDown(KeyCode::A) || input->IsKeyDown(KeyCode::LEFT))
    {
        movement -= 1.0f;
    }

    if (input->IsKeyDown(KeyCode::D) || input->IsKeyDown(KeyCode::RIGHT))
    {
        movement += 1.0f;
    }

    mPlayerPosition.x += movement * mPlayerSpeed * deltaTime;

    if (mPlayerPosition.x < -mArenaHalfWidth)
    {
        mPlayerPosition.x = -mArenaHalfWidth;
    }

    if (mPlayerPosition.x > mArenaHalfWidth)
    {
        mPlayerPosition.x = mArenaHalfWidth;
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
            ++mScore;
            ++asteroid.resetCount;
            ResetAsteroid(i);
            continue;
        }

        if (CheckCollision(asteroid))
        {
            mGameOver = true;
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

bool GameState::CheckCollision(const Asteroid& asteroid) const
{
    const float dx = asteroid.position.x - mPlayerPosition.x;
    const float dz = asteroid.position.z - mPlayerPosition.z;

    const float distanceSquared = (dx * dx) + (dz * dz);
    const float radiusSum = asteroid.radius + mPlayerRadius;

    return distanceSquared <= radiusSum * radiusSum;
}

void GameState::Render()
{
    SimpleDraw::AddGroundPlane(12.0f, Colors::DarkSeaGreen);

    // Arena boundaries
    SimpleDraw::AddLine({ -mArenaHalfWidth, 0.05f, mDeathZ }, { -mArenaHalfWidth, 0.05f, mSpawnZ }, Colors::White);
    SimpleDraw::AddLine({ mArenaHalfWidth, 0.05f, mDeathZ }, { mArenaHalfWidth, 0.05f, mSpawnZ }, Colors::White);
    SimpleDraw::AddLine({ -mArenaHalfWidth, 0.05f, mDeathZ }, { mArenaHalfWidth, 0.05f, mDeathZ }, Colors::White);
    SimpleDraw::AddLine({ -mArenaHalfWidth, 0.05f, mSpawnZ }, { mArenaHalfWidth, 0.05f, mSpawnZ }, Colors::White);

    // Player
    const Color playerColor = mGameOver ? Colors::Red : Colors::Cyan;
    SimpleDraw::AddSphere(16, 16, mPlayerRadius, mPlayerPosition, playerColor);

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

    if (mGameOver)
    {
        ImGui::Separator();
        ImGui::Text("GAME OVER");
        ImGui::Text("Press R to restart.");
    }

    ImGui::Separator();

    if (ImGui::Button("Restart Game"))
    {
        ResetGame();
    }

    ImGui::Checkbox("Use Parallel Update", &mUseParallelUpdate);

    if (ImGui::SliderInt("Asteroid Count", &mAsteroidCount, 10, 10000))
    {
        ResetGame();
    }

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