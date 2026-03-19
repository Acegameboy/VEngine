#pragma once
#include <VEngine/Inc/VEngine.h>
#include <vector>
#include <functional>


namespace Char1Clip { enum { Idle = 0, Running, Jump, Dying }; }
namespace Char2Clip { enum { Idle = 0, Running, SillyDancing, Jump }; }

struct EventKeyframe
{
    float triggerTime = 0.0f;
    bool  fired = false;
    std::function<void()> action;
};

struct Particle
{
    VEngine::Math::Vector3   position;
    VEngine::Math::Vector3   velocity;
    VEngine::Color color;
    float lifetime = 0.0f;
    float maxLife = 1.0f;
    float size = 0.05f;
};

class GameState : public VEngine::AppState
{
public:
    void Initialize() override;
    void Terminate()  override;
    void Update(float deltaTime) override;
    void Render()     override;
    void DebugUI()    override;

private:
    void UpdateCamera(float deltaTime);
    void UpdateCharacterMovement(float deltaTime);
    void UpdateParticles(float deltaTime);
    void RenderParticles();
    void SpawnThrusterParticles(const VEngine::Math::Vector3& pos,
        const VEngine::Math::Vector3& dir,
        const VEngine::Color& col,
        int count = 4);
    void SpawnExplosion(const VEngine::Math::Vector3& pos,
        const VEngine::Color& col,
        int count = 60);
    void FireEventsUpTo(float time);
    void ResetScene();

    VEngine::Graphics::RenderGroup  mCharacter01;   
    VEngine::Graphics::RenderGroup  mCharacter02;   
    VEngine::Graphics::Animator     mAnimator01;
    VEngine::Graphics::Animator     mAnimator02;

    VEngine::Graphics::StandardEffect   mStandardEffect;
    VEngine::Graphics::DirectionalLight mDirectionalLight;
    VEngine::Graphics::Camera           mCamera;

    VEngine::Math::Vector3 mChar1Pos = { -3.0f, 0.0f, 0.0f };
    VEngine::Math::Vector3 mChar2Pos = { 3.0f, 0.0f, 0.0f };
    float mChar1Yaw = 0.0f;   
    float mChar2Yaw = 3.14159f;

    std::vector<Particle> mParticles;

    std::vector<EventKeyframe> mEvents;

    float mSceneTime = 0.0f;
    float mAnimSpeed = 1.0f;

    bool mRedExploded = false;
    bool mSceneRunning = true;
    bool mScenePaused = false;

    bool mDrawSkeleton01 = false;
    bool mDrawSkeleton02 = false;

    static constexpr float kSceneDuration = 50.0f;  
    static constexpr int   kMaxParticles = 2000;
};