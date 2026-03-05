#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Physics;


void GameState::Initialize()
{
    mCamera.SetPosition({ 2.0f, 2.0f, -2.0f });
    mCamera.SetLookAt({ 0.0f, 1.2f, 0.0f });

    mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    TextureManager* tm = TextureManager::Get();

    // Ground
    Mesh plane = MeshBuilder::CreatePlane(20, 20, 1.0f, true);
    mGroundObject.meshBuffer.Initialize(plane);
    mGroundShape.InitializeHull({ 5.0f, 0.5f, 5.0f }, { 0.0f, -0.5f, 0.0f });
    mGroundRigidBody.Initialize(mGroundObject.transform, mGroundShape, 0.0f);

    mGroundObject.diffuseMapId = tm->LoadTexture(L"misc/concrete.jpg");

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    uint32_t numSegments = 10;
    float maxSphereSize = 1.0f;
    float minSphereSize = 0.25f;
    for (uint32_t i = 0; i < numSegments; ++i)
    {
        const float t = (1.0f - static_cast<float>(i) / static_cast<float>(numSegments - 1));
        float sphereSize = Math::Lerp(minSphereSize, maxSphereSize, t);
        Mesh sphere = MeshBuilder::CreateSphere(20, 20, sphereSize);
        Segment& part = mSegments.emplace_back();
        part.segment.meshBuffer.Initialize(sphere);
        part.segment.diffuseMapId = tm->LoadTexture("Images/blue.jpg");
        part.range = sphereSize;
        if (i > 0)
        {
            Math::Vector3 prevPos = mSegments[i - 1].segment.transform.position;
            float prevRange = mSegments[i - 1].range;
            part.segment.transform.position.x -= (prevPos.x - prevRange);
        }
    }
    mBodyAnchorShape.InitializeSphere(maxSphereSize);
    mBodyAnchor.Initialize(mSegments[0].segment.transform, mBodyAnchorShape, 1.0f);

    
}

void GameState::Terminate()
{
    mBodyAnchor.Terminate();
    mBodyAnchorShape.Terminate();

    for (Segment& segment : mSegments)
    {
        segment.segment.Terminate();
    }
    mSegments.clear();
    mStandardEffect.Terminate();

    mGroundRigidBody.Terminate();
    mGroundShape.Terminate();
    mGroundObject.Terminate();

    
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
    InputSystem* input = InputSystem::Get();
    if (InputSystem::Get()->IsKeyPressed(KeyCode::SPACE))
    {
        Math::Vector3 spawnPos = mCamera.GetPosition() + (mCamera.GetDirection() * 0.5f);
        mBodyAnchor.SetPosition(spawnPos);
        mBodyAnchor.SetVelocity(mCamera.GetDirection() * 20.0f);
    }
    Math::Vector3 velocity = Math::Vector3::Zero;
    velocity.y = mBodyAnchor.GetVelocity().y;
    if (input->IsKeyDown(KeyCode::UP))
    {
        velocity.z += 1.0f;
    }
    if (input->IsKeyDown(KeyCode::DOWN))
    {
        velocity.z -= 1.0f;
    }
    if (input->IsKeyDown(KeyCode::RIGHT))
    {
        velocity.x += 1.0f;
    }
    if (input->IsKeyDown(KeyCode::LEFT))
    {
        velocity.x -= 1.0f;
    }
    if (velocity.y < 1.0f && input->IsKeyDown(KeyCode::SPACE))
    {
        velocity.y = 10.0f;
    }
    mBodyAnchor.SetVelocity(velocity);
}

void GameState::Render()
{
    //Start at 1 because 0 is managed by physics
    //also starting at 1 means that previous value is accessible
    for (uint32_t i = 1; i < mSegments.size(); ++i)
    {
        Segment& prevSeg = mSegments[i - 1];
        Segment& seg = mSegments[i];

        //PSEUDO WAY OF ""COLLIDING WITH GROUND"
        seg.segment.transform.position.y = Math::Max(seg.segment.transform.position.y, seg.range);
        Math::Vector3 dir = seg.segment.transform.position - prevSeg.segment.transform.position;
        if (Math::MagnitudeSqr(dir) > 0.0f)
        {
            dir = Math::Normalize(dir);
        }
        else
        {
            dir = Math::Vector3::ZAxis;
        }
        //pushing segments to proper locations
        seg.segment.transform.position = prevSeg.segment.transform.position + (dir * prevSeg.range);
        seg.segment.transform.position.y = Math::Max(seg.segment.transform.position.y, seg.range);
    }
    mStandardEffect.Begin();
    mStandardEffect.Render(mGroundObject);
    for (const Segment& segment : mSegments)
    {
        mStandardEffect.Render(segment.segment);
    }
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

    ImGui::Separator();

    mStandardEffect.DebugUI();
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
}