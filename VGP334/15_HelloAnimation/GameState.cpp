#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

void GameState::Initialize()
{
    mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    Mesh mesh = MeshBuilder::CreateSphere(30, 30, 0.25f);
    mRenderObject.meshBuffer.Initialize(mesh);

    TextureManager* tm = TextureManager::Get();
    mRenderObject.diffuseMapId = tm->LoadTexture(L"earth.jpg");

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    mAnimationTime = 0.0f;
    mAnimation = AnimationBuilder()
        .AddPositionKeys({ 0.0f, 0.0f, 0.0f }, 0.0f)
        .AddPositionKeys({ 0.0f, 2.0f, 0.0f }, 3.0f)
        .AddPositionKeys({ 0.0f, 0.0f, 0.0f }, 5.0f)
        .AddRotationKeys(Math::Quaternion::Identity, 0.0f)
        .AddRotationKeys(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 90.0f * Math::Constants::DegToRad), 2.0f)
        .AddRotationKeys(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 180.0f * Math::Constants::DegToRad), 3.0f)
        .AddRotationKeys(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 270.0f * Math::Constants::DegToRad), 4.0f)
        .AddRotationKeys(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 359.0f * Math::Constants::DegToRad), 5.0f)
        .AddScaleKeys(Math::Vector3::One, 0.0f)
        .AddScaleKeys(Math::Vector3::One * 0.1f, 3.0f)
        .AddScaleKeys(Math::Vector3::One, 5.0f)
        .Build();
}

void GameState::Terminate()
{
    mRenderObject.Terminate();
    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);

    mAnimationTime += deltaTime;
    while (mAnimationTime > mAnimation.GetDuration())
    {
        mAnimationTime -= mAnimation.GetDuration();
    }
}

void GameState::Render()
{
    mRenderObject.transform = mAnimation.GetTransform(mAnimationTime);
    SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
    SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();
    mStandardEffect.Render(mRenderObject);
    mStandardEffect.End();
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

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ColorEdit4("Emissive#Material", &mRenderObject.material.emissive.r);
        ImGui::ColorEdit4("Ambient#Material", &mRenderObject.material.ambient.r);
        ImGui::ColorEdit4("Diffuse#Material", &mRenderObject.material.diffuse.r);
        ImGui::ColorEdit4("Specular#Material", &mRenderObject.material.specular.r);
        ImGui::DragFloat(
            "Shininess#Material", &mRenderObject.material.shininess, 0.01f, 0.0f, 10000.0f);
    }

    mStandardEffect.DebugUI();
    ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
    InputSystem* input = InputSystem::Get();
    const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
    const float turnSpeed = 0.5f;

    if (input->IsKeyDown(KeyCode::W))
    {
        mCamera.Walk(moveSpeed * deltaTime);
    }
    else if (input->IsKeyDown(KeyCode::S))
    {
        mCamera.Walk(-moveSpeed * deltaTime);
    }
    else if (input->IsKeyDown(KeyCode::D))
    {
        mCamera.Strafe(moveSpeed * deltaTime);
    }
    else if (input->IsKeyDown(KeyCode::A))
    {
        mCamera.Strafe(-moveSpeed * deltaTime);
    }
    else if (input->IsKeyDown(KeyCode::E))
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
