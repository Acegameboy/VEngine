#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Physics;


void GameState::Initialize()
{
    mCamera.SetPosition({ 2.0f, 2.0f, -2.0f });
    mCamera.SetLookAt({ 0.0f, 1.2f, 0.0f });

    mParticleSystemEffect.Initialize();
    mParticleSystemEffect.SetCamera(mCamera);

    ParticleSystemInfo info;
    info.textureId = TextureManager::Get()->LoadTexture("Images/white.jpg");
    info.maxParticles = 1000;
    info.particlesPerEmit = { 1,4 };
    info.delay = 1.0f;
    info.lifeTime = FLT_MAX;
    info.timeBetweenEmit = { 0.2f, 0.4f };
    info.spaenAngle = { -30.0f, 30.0f };
    info.spawnSpeed = { 1.0f, 3.0f };
    info.partilceLifeTime = { 0.5f, 2.0f };
    info.spawnDirection = Math::Vector3::YAxis;
    info.spawnPosition = Math::Vector3::Zero;
    info.startScale = { Math::Vector3::One, Math::Vector3::One };
    info.endScale = { Math::Vector3::One, Math::Vector3::One };
    info.startColor = { Colors::White, Colors::White };
    mParticleSystem.Initialize(info);
}

void GameState::Terminate()
{
    mParticleSystem.Terminate();
    mParticleSystemEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
    mParticleSystem.Update(deltaTime);

    
}

void GameState::Render()
{
    SimpleDraw::AddGroundPlane(10.0F, Colors::White);
    SimpleDraw::Render(mCamera);

    mParticleSystemEffect.Begin();
    mParticleSystemEffect.Render();
    mParticleSystemEffect.End();
}

void GameState::DebugUI()
{
    ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    mParticleSystem
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
}