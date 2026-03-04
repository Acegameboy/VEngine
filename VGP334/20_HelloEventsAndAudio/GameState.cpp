#include "GameState.h"
#include "GameEvents.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Physics;
using namespace VEngine::Core;
using namespace VEngine::Audio;

void GameState::Initialize()
{
    mCamera.SetPosition({ 2.0f, 2.0f, -2.0f });
    mCamera.SetLookAt({ 0.0f, 1.2f, 0.0f });

    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    // Particles
    mParticleSystemEffect.Initialize();
    mParticleSystemEffect.SetCamera(mCamera);

    ParticleSystemInfo info;
    info.textureId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/Images/bullet2.png");
    info.maxParticles = 1000;
    info.particlesPerEmit = { 8 ,12 };
    info.delay = 1.0f;
    info.lifeTime = FLT_MAX;
    info.timeBetweenEmit = { 0.1f, 0.2f };
    info.spawnAngle = { -20.0f, 20.0f };
    info.spawnSpeed = { 1.0f, 3.0f };
    info.particleLifeTime = { 0.5f, 2.0f };
    info.spawnDirection = -Math::Vector3::YAxis;
    info.spawnPosition = Math::Vector3::Zero;
    info.startScale = { Math::Vector3::One, Math::Vector3::One };
    info.endScale = { Math::Vector3::One, Math::Vector3::One };
    info.startColor = { Colors::White, Colors::White };
    info.endColor = { Colors::White, Colors::White };
    mParticleSystem.Initialize(info);

    info.textureId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/Images/mushroom.png");
    info.particlesPerEmit = { 10, 250 };
    info.spawnSpeed = { 7.0f, 25.0f };
    info.delay = 0.0f;
    info.lifeTime = 0.0f;
    info.spawnAngle = { -180.0f, 180.0f };
    info.spawnDirection = Math::Vector3::YAxis;
    info.startColor = { Colors::OrangeRed, Colors::LightYellow };
    info.endColor = { Colors::LightGray, Colors::White };
    mFireworkParticles.Initialize(info);

    // Events
    EventManager* em = EventManager::Get();
    mSpacePressedListenerId = em->AddListener(PressSpaceEvent::StaticGetTypeId(),
        std::bind(&GameState::OnSpacePressedEvent, this, std::placeholders::_1));

    mEnterPressedListenerId = em->AddListener(PressEnterEvent::StaticGetTypeId(),
        [](const Event& e)
        {
            LOG("E N T E R  WAZ HERE!");
        });

    // Sounds
    SoundEffectManager* sm = SoundEffectManager::Get();
    mLaunchSoundId = sm->Load("megamanx_blast.wav");
    mExplosionSoundId = sm->Load("explosion.wav");

    auto launchFirework = [&]()
        {
            SoundEffectManager::Get()->Play(mLaunchSoundId);
        };

    auto spawnFirework = [&]()
        {
            mFireworkParticles.SpawnParticles();
            SoundEffectManager::Get()->Play(mExplosionSoundId);
        };
    mFireworkAnimation = AnimationBuilder()
        .AddPositionKeys(Math::Vector3::Zero, 0.0f)
        .AddPositionKeys(Math::Vector3::Zero, 2.0f)
        .AddPositionKeys(Math::Vector3::YAxis * 25.0f, 4.0f)
        .AddEventKeys(launchFirework, 2.0f)
        .AddEventKeys(spawnFirework, 4.0f)
        .Build();
}

void GameState::Terminate()
{
    SoundEffectManager* sm = SoundEffectManager::Get();
    sm->Stop(mLaunchSoundId);
    sm->Stop(mExplosionSoundId);

    EventManager* em = EventManager::Get();
    em->RemoveListener(PressSpaceEvent::StaticGetTypeId(), mSpacePressedListenerId);
    em->RemoveListener(PressEnterEvent::StaticGetTypeId(), mEnterPressedListenerId);

    mFireworkParticles.Terminate();

    mParticleSystem.Terminate();
    mParticleSystemEffect.Terminate();

    mCloth.Terminate();
    mClothSoftBody.Terminate();

    for (BoxData& box : mBoxes)
    {
        box.rigidBody.Terminate();
        box.shape.Terminate();
        box.box.Terminate();
    }

    mStandardEffect.Terminate();

    mGroundRigidBody.Terminate();
    mGroundShape.Terminate();
    mGroundObject.Terminate();

    mBallRigidBody.Terminate();
    mBallShape.Terminate();
    mFootball.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
    if (InputSystem::Get()->IsKeyPressed(KeyCode::SPACE))
    {
        Math::Vector3 spawnPos = mCamera.GetPosition() + (mCamera.GetDirection() * 0.5f);
        mBallRigidBody.SetPosition(spawnPos);
        mBallRigidBody.SetVelocity(mCamera.GetDirection() * 50.0f);
    }

    mParticleSystem.Update(deltaTime);
    mFireworkParticles.Update(deltaTime);

    float prevTime = mFireworkAnimationTime;
    mFireworkAnimationTime += deltaTime;
    if (mFireworkAnimation.GetDuration() > 0)
    {
        mFireworkAnimation.PlayEvents(prevTime, mFireworkAnimationTime);
        while (mFireworkAnimationTime >= mFireworkAnimation.GetDuration())
        {
            mFireworkAnimationTime -= mFireworkAnimation.GetDuration();
        }
    }
}

void GameState::Render()
{
    mCloth.meshBuffer.Update(mClothMesh.vertices.data(), mClothMesh.vertices.size());
    SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
    SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();
    mStandardEffect.Render(mFootball);
    mStandardEffect.Render(mGroundObject);
    mStandardEffect.Render(mCloth);
    for (BoxData& box : mBoxes)
    {
        mStandardEffect.Render(box.box);
    }
    mStandardEffect.End();

    Transform particleTransform = mFireworkAnimation.GetTransform(mFireworkAnimationTime);
    mParticleSystem.SetPosition(particleTransform.position);
    mFireworkParticles.SetPosition(particleTransform.position);

    mParticleSystemEffect.Begin();
    mParticleSystem.Render(mParticleSystemEffect);
    mFireworkParticles.Render(mParticleSystemEffect);
    mParticleSystemEffect.End();
}

void GameState::DebugUI()
{
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
        {
            mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
        }

        ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
        ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
        ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
    }

    ImGui::Separator();

    Math::Vector3 pos = mFootball.transform.position;
    if (ImGui::DragFloat3("BallPosition", &pos.x))
    {
        mFootball.transform.position = pos;
        mBallRigidBody.SetPosition(mFootball.transform.position);
    }

    mStandardEffect.DebugUI();
    mParticleSystem.DebugUI();
    PhysicsWorld::Get()->DebugUI();
    ImGui::End();
    SimpleDraw::Render(mCamera);
}

void GameState::UpdateCamera(float deltaTime)
{
    // Camera Controls:
    InputSystem* input = InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
    const float turnSpeed = 0.5f;

    if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }

    else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime); }

    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
        mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
    }

    if (input->IsKeyPressed(KeyCode::SPACE))
    {
        PressSpaceEvent event;
        EventManager::Broadcast(event);
    }

    if (input->IsKeyPressed(KeyCode::ENTER))
    {
        PressEnterEvent event;
        EventManager::Broadcast(event);
    }
}

void GameState::OnSpacePressedEvent(const VEngine::Core::Event& e)
{
    LOG("S P A C E  WAS PRESSED!");
    mFireworkParticles.SpawnParticles();
}