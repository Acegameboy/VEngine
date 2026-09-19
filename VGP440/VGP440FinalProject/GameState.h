#pragma once

#include <VEngine/Inc/VEngine.h>

#include <Network/Inc/Common.h>
#include <Network/Inc/Server.h>
#include <Network/Inc/Client.h>

#include "NetworkPacket.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <array>

class GameState : public VEngine::AppState
{
public:
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Render() override;
    void DebugUI() override;

private:
    struct Asteroid
    {
        VEngine::Math::Vector3 position = VEngine::Math::Vector3::Zero;
        VEngine::Math::Vector3 velocity = VEngine::Math::Vector3::Zero;
        float radius = 0.15f;
        uint32_t resetCount = 0;
    };

    struct PlayerState
    {
        VEngine::Math::Vector3 position =
            VEngine::Math::Vector3::Zero;

        float radius = 0.25f;

        float survivalTime = 0.0f;

        bool alive = true;
    };

    class ThreadPool
    {
    public:
        ThreadPool() = default;
        ~ThreadPool();

        void Start(uint32_t threadCount);
        void Stop();

        void AddTask(std::function<void()> task);
        void Wait();

        uint32_t GetThreadCount() const { return mThreadCount; }

    private:
        void WorkerLoop();

        std::vector<std::thread> mWorkers;
        std::queue<std::function<void()>> mTasks;

        std::mutex mQueueMutex;
        std::condition_variable mQueueCondition;

        std::mutex mWaitMutex;
        std::condition_variable mWaitCondition;

        bool mRunning = false;
        uint32_t mThreadCount = 0;
        uint32_t mActiveTasks = 0;
    };

private:
    void ResetGame();
    void UpdateCamera(float deltaTime);
    void UpdatePlayer(float deltaTime);


    void UpdateAsteroidsParallel(float deltaTime);
    void UpdateAsteroidRange(size_t startIndex, size_t endIndex, float deltaTime);
    void ResetAsteroid(size_t index);

    void ApplyMovement(PlayerState& player, float movement, float deltaTime);
    void CheckPlayerCollisions();
    bool CheckCollision(const Asteroid& asteroid, const PlayerState& player) const;
    void SendGameState();
    void SendAsteroidSnapshot();
    void ProcessServerPacket(const char* data, int dataLength);
    void ProcessClientPacket(const char* data, int dataLength);

    void StartHost();
    void StartClient();
    void StopNetwork();
    void UpdateNetwork(float deltaTime);

private:
    VEngine::Graphics::Camera mCamera;

    ThreadPool mThreadPool;

    std::vector<Asteroid> mAsteroids;

    std::array<PlayerState, 2> mPlayers;

    float mPlayerSpeed = 4.0f;

    float mArenaHalfWidth = 5.0f;
    float mSpawnZ = 8.0f;
    float mDeathZ = -4.0f;

    int mAsteroidCount = 600;

    std::atomic<int> mScore = 0;
    std::atomic<bool> mGameOver = false;

    bool mUseParallelUpdate = true;

    enum class NetworkMode
    {
        None,
        Host,
        Client
    };

    std::unique_ptr<VEngine::Network::Server> mServer;
    std::unique_ptr<VEngine::Network::Client> mClient;

    NetworkMode mNetworkMode = NetworkMode::None;

    int mPlayerID = -1;

    bool mNetworkConnected = false;

    std::string mNetworkStatus = "Not connected";

    char mServerAddress[64] = "127.0.0.1";

    float mRemoteMovement = 0.0f;

    bool mMatchStarted = false;
    bool mMatchOver = false;

    int mWinner = 0;

    float mStateBroadcastTimer = 0.0f;
    float mAsteroidBroadcastTimer = 0.0f;

    const float mStateBroadcastRate = 0.05f;
    const float mAsteroidBroadcastRate = 0.10f;

};