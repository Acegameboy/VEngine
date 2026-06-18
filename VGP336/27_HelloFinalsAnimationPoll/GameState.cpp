#include "GameState.h"
#include "AnimationReactionSystem.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

void GameState::Initialize()
{
    mGameWorld.LoadLevel("../../Assets/Templates/Levels/level.json");

    mPlayer = mGameWorld.FindGameObject("Character01");
    mNpc = mGameWorld.FindGameObject("Character02");

    ASSERT(mPlayer != nullptr, "GameState: Character01 not found in level.");
    ASSERT(mNpc != nullptr, "GameState: Character02 not found in level.");

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