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
    auto* input = VEngine::Input::InputSystem::Get();

    if (input->IsKeyPressed(VEngine::Input::KeyCode::ONE))
    {
        mReactionSystem.TriggerTaunt();
    }

    if (input->IsKeyPressed(VEngine::Input::KeyCode::TWO))
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

    ImGui::Text("Press 1 = Random Player Taunt / Random NPC Attack");
    ImGui::Text("Press 2 = Random Player Greet / Random NPC Greet");

    ImGui::Separator();

    if (ImGui::Button("Random Taunt Reaction"))
    {
        mReactionSystem.TriggerTaunt();
    }

    if (ImGui::Button("Random Greet Reaction"))
    {
        mReactionSystem.TriggerGreet();
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