#pragma once

#include <VEngine/Inc/VEngine.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

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
    bool CheckCollision(const Asteroid& asteroid) const;

private:
    VEngine::Graphics::Camera mCamera;

    ThreadPool mThreadPool;

    std::vector<Asteroid> mAsteroids;

    VEngine::Math::Vector3 mPlayerPosition = VEngine::Math::Vector3::Zero;
    float mPlayerRadius = 0.25f;
    float mPlayerSpeed = 4.0f;

    float mArenaHalfWidth = 5.0f;
    float mSpawnZ = 8.0f;
    float mDeathZ = -4.0f;

    int mAsteroidCount = 600;

    std::atomic<int> mScore = 0;
    std::atomic<bool> mGameOver = false;

    bool mUseParallelUpdate = true;
};