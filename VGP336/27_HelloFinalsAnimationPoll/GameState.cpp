#include "GameState.h"
#include "AnimationReactionSystem.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

void GameState::Initialize()
{
    mGameWorld.AddService<CameraService>();
    mGameWorld.AddService<RenderService>();

    mGameWorld.Initialize(100);

    std::filesystem::path cameraPath =
        "../../Assets/Templates/Objects/fps_camera.json";

    std::filesystem::path playerPath =
        "../../Assets/Templates/Objects/character01.json";

    std::filesystem::path npcPath =
        "../../Assets/Templates/Objects/character02.json";

    ASSERT(std::filesystem::exists(cameraPath),
        "Missing camera template file: %s",
        cameraPath.u8string().c_str());

    ASSERT(std::filesystem::exists(playerPath),
        "Missing player template file: %s",
        playerPath.u8string().c_str());

    ASSERT(std::filesystem::exists(npcPath),
        "Missing NPC template file: %s",
        npcPath.u8string().c_str());

    mCameraObject = mGameWorld.CreateGameObject(
        "MainCamera",
        cameraPath);

    mPlayer = mGameWorld.CreateGameObject(
        "Character1_Player",
        playerPath);

    mNpc = mGameWorld.CreateGameObject(
        "Character2_NPC",
        npcPath);

    ASSERT(mCameraObject != nullptr, "Failed to create camera.");
    ASSERT(mPlayer != nullptr, "Failed to create player.");
    ASSERT(mNpc != nullptr, "Failed to create NPC.");

    mCameraObject->Initialize();
    mPlayer->Initialize();
    mNpc->Initialize();

    TransformComponent* playerTransform =
        mPlayer->GetComponent<TransformComponent>();

    TransformComponent* npcTransform =
        mNpc->GetComponent<TransformComponent>();

    if (playerTransform != nullptr)
    {
        playerTransform->position = { -1.5f, 0.0f, 0.0f };
        playerTransform->scale = { 1.0f, 1.0f, 1.0f };
    }

    if (npcTransform != nullptr)
    {
        npcTransform->position = { 1.5f, 0.0f, 0.0f };
        npcTransform->scale = { 1.0f, 1.0f, 1.0f };
    }

    mReactionSystem.Initialize(mPlayer, mNpc);
}

void GameState::Terminate()
{
    mGameWorld.Terminate();

    mCameraObject = nullptr;
    mPlayer = nullptr;
    mNpc = nullptr;
}

void GameState::Update(float deltaTime)
{
    InputSystem* input = InputSystem::Get();

    if (input->IsKeyPressed(KeyCode::ONE))
    {
        mReactionSystem.TriggerTaunt();
    }

    if (input->IsKeyPressed(KeyCode::TWO))
    {
        mReactionSystem.TriggerGreet();
    }

    mReactionSystem.Update(deltaTime);

    mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
    mGameWorld.Render();
}

void GameState::DebugUI()
{
    ImGui::Begin("Animation Pool Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    if (ImGui::Button("Reaction: Player Taunt / NPC Attack"))
    {
        mReactionSystem.TriggerTaunt();
    }

    if (ImGui::Button("Reaction: Player Greet / NPC Greet"))
    {
        mReactionSystem.TriggerGreet();
    }

    ImGui::Separator();

    AnimatorComponent* playerAnimator =
        mPlayer->GetComponent<AnimatorComponent>();

    AnimatorComponent* npcAnimator =
        mNpc->GetComponent<AnimatorComponent>();

    if (ImGui::Button("Direct Play Player Anim 0"))
    {
        bool result = playerAnimator->Play(0, true);
        LOG("Direct Player Play(0) = %s", result ? "true" : "false");
    }

    if (ImGui::Button("Direct Play Player Anim 1"))
    {
        bool result = playerAnimator->Play(1, false);
        LOG("Direct Player Play(1) = %s", result ? "true" : "false");
    }

    if (ImGui::Button("Direct Play NPC Anim 0"))
    {
        bool result = npcAnimator->Play(0, true);
        LOG("Direct NPC Play(0) = %s", result ? "true" : "false");
    }

    if (ImGui::Button("Direct Play NPC Anim 1"))
    {
        bool result = npcAnimator->Play(1, false);
        LOG("Direct NPC Play(1) = %s", result ? "true" : "false");
    }

    ImGui::End();

    mGameWorld.DebugUI();
}

void GameState::UpdateAnimationInput()
{
    VEngine::Input::InputSystem* input =
        VEngine::Input::InputSystem::Get();

    if (input->IsKeyPressed(VEngine::Input::KeyCode::ONE))
    {
        mReactionSystem.TriggerTaunt();
    }

    if (input->IsKeyPressed(VEngine::Input::KeyCode::TWO))
    {
        mReactionSystem.TriggerGreet();
    }
}