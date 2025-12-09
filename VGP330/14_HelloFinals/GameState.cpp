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

    mCharacter01.Initialize("Character01/Character01.model");
    mCharacter02.Initialize("Character02/Character02.model");
    mCharacter02.transform.position = { -2.0f, 0.0f, 0.0f };
    mCharacter03.Initialize("Character03/Character03.model");
    mCharacter03.transform.position = { 2.0f, 0.0f, 0.0f };

    mTerrain.Initialize(L"../../Assets/Textures/terrain/heightmap_512x512.raw", 10.0f);
    mGround.meshBuffer.Initialize(mTerrain.mesh);

    mGround.diffuseMapId = TextureManager::Get()->LoadTexture("terrain/dirt_seamless.jpg");
    mGround.specMapId = TextureManager::Get()->LoadTexture("terrain/grass_2048.jpg");


    TextureManager* tm = TextureManager::Get();

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);
    mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
    mStandardEffect.SetShadowMap(mShadowEffect.GetDepthMap());

    // Thermal: enable for now, for everything using StandardEffect
    mStandardEffect.SetThermalEnabled(true);
    mStandardEffect.SetThermalBaseHeat(0.5f);
    mStandardEffect.SetThermalVariation(0.5f);

    mShadowEffect.Initialize();
    mShadowEffect.SetDirectionalLight(mDirectionalLight);

    mTerrainEffect.Initialize();
    mTerrainEffect.SetCamera(mCamera);
    mTerrainEffect.SetLightCamera(mShadowEffect.GetLightCamera());
    mTerrainEffect.SetDirectionalLight(mDirectionalLight);
    mTerrainEffect.SetShadowMap(mShadowEffect.GetDepthMap());
}

void GameState::Terminate()
{
    mTerrainEffect.Terminate();
    mShadowEffect.Terminate();
    mCharacter01.Terminate();
    mCharacter02.Terminate();
    mCharacter03.Terminate();
    mStandardEffect.Terminate();
    mGround.Terminate();
}

void GameState::Update(float deltaTime)
{
    UpdateCamera(deltaTime);
}

void GameState::Render()
{
    //SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
    //SimpleDraw::Render(mCamera);
    
    mShadowEffect.Begin();
    mShadowEffect.Render(mCharacter01);
    mShadowEffect.Render(mCharacter02);
    mShadowEffect.Render(mCharacter03);
    mShadowEffect.End();

    mTerrainEffect.Begin();
    mTerrainEffect.Render(mGround);
    mTerrainEffect.End();

    mStandardEffect.Begin();
    mStandardEffect.Render(mCharacter01);
    mStandardEffect.Render(mCharacter02);
    mStandardEffect.Render(mCharacter03);
    mStandardEffect.Render(mGround);
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
        for (uint32_t i = 0; i < mCharacter01.renderObjects.size(); ++i)
        {
            Material& material = mCharacter01.renderObjects[i].material;
            std::string renderObjectId = "RenderObject" + std::to_string(i);
            ImGui::PushID(renderObjectId.c_str());
            if(ImGui::CollapsingHeader(renderObjectId.c_str()))
            {
                ImGui::ColorEdit4("Emissive#Material", &material.emissive.r);
                ImGui::ColorEdit4("Ambient#Material", &material.ambient.r);
                ImGui::ColorEdit4("Diffuse#Material", &material.diffuse.r);
                ImGui::ColorEdit4("Specular#Material", &material.specular.r);
                ImGui::DragFloat("Shiness#Material", &material.shininess, 0.1f, 0.1f, 1000.0f);
            }
            ImGui::PopID();
        }
    }
    ImGui::DragFloat3("CharacterPosition", &mCharacter01.transform.position.x);
    mStandardEffect.DebugUI();
    mShadowEffect.DebugUI();
    mTerrainEffect.DebugUI();
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

    Math::Vector3 cameraPosition = mCamera.GetPosition();
    float height = mTerrain.GetHeight(mCamera.GetPosition());
    if (height >= 0.0f)
    {
        cameraPosition.y = height + 1.5f;
        mCamera.SetPosition(cameraPosition);
    }
}
