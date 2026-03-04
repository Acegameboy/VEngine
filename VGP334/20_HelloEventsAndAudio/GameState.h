#pragma once

#include <VEngine/Inc/VEngine.h>

class GameState : public VEngine::AppState
{
public:
	void Initialize() override;

	void Terminate() override;

	void Update(float deltaTime) override;

	void Render() override;

	void DebugUI() override;

private:

    void UpdateCamera(float deltaTime);

    void OnSpacePressedEvent(const VEngine::Core::Event& e);

    VEngine::Graphics::Camera mCamera;
    VEngine::Graphics::DirectionalLight mDirectionalLight;

    // Events
    VEngine::Core::EventListenerId mSpacePressedListenerId = 0;
    VEngine::Core::EventListenerId mEnterPressedListenerId = 0;

    // Particles
    VEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;
    VEngine::Physics::ParticleSystem mParticleSystem;
    VEngine::Physics::ParticleSystem mFireworkParticles;

    // Animation
    VEngine::Graphics::Animation mFireworkAnimation;
    float mFireworkAnimationTime = 0.0f;

    // Ball info
    VEngine::Graphics::RenderObject mFootball;
    VEngine::Physics::CollisionShape mBallShape;
    VEngine::Physics::RigidBody mBallRigidBody;

    // Ground info
    VEngine::Graphics::RenderObject mGroundObject;
    VEngine::Physics::CollisionShape mGroundShape;
    VEngine::Physics::RigidBody mGroundRigidBody;

    struct BoxData
    {
        VEngine::Graphics::RenderObject box;
        VEngine::Physics::CollisionShape shape;
        VEngine::Physics::RigidBody rigidBody;
    };
    using Boxes = std::vector<BoxData>;
    Boxes mBoxes;

    VEngine::Graphics::StandardEffect mStandardEffect;

    // Cloth info
    VEngine::Graphics::RenderObject mCloth;
    VEngine::Graphics::Mesh mClothMesh;
    VEngine::Physics::SoftBody mClothSoftBody;

    // Sound
    VEngine::Audio::SoundId mLaunchSoundId = 0;
    VEngine::Audio::SoundId mExplosionSoundId = 0;
};