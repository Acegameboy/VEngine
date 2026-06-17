#include "AnimationReactionSystem.h"

using namespace VEngine;

void AnimationReactionSystem::Initialize(
    GameObject* player,
    GameObject* npc)
{
    mPlayer = player;
    mNpc = npc;

    ASSERT(mPlayer != nullptr,
        "AnimationReactionSystem: Player is null.");

    ASSERT(mNpc != nullptr,
        "AnimationReactionSystem: NPC is null.");

    mPlayerAnimator = mPlayer->GetComponent<AnimatorComponent>();
    mNpcAnimator = mNpc->GetComponent<AnimatorComponent>();

    ASSERT(mPlayerAnimator != nullptr,
        "AnimationReactionSystem: Player needs an AnimatorComponent.");

    ASSERT(mNpcAnimator != nullptr,
        "AnimationReactionSystem: NPC needs an AnimatorComponent.");

    const uint32_t playerAnimCount =
        mPlayerAnimator->GetAnimator().GetAnimationCount();

    const uint32_t npcAnimCount =
        mNpcAnimator->GetAnimator().GetAnimationCount();

    LOG("Player animation count = %d", playerAnimCount);
    LOG("NPC animation count = %d", npcAnimCount);

    mPlayerAnimations.idle = 0;
    mPlayerAnimations.taunt = 1;
    mPlayerAnimations.greet = 1;

    mNpcAnimations.idle = 0;
    mNpcAnimations.attack = 1;
    mNpcAnimations.greet = 1;

    ReturnToIdle();
}

void AnimationReactionSystem::Update(float deltaTime)
{
    if (mCurrentAction == Action::None)
    {
        return;
    }

    mReactionTimer += deltaTime;

    if (mReactionTimer >= mReactionDuration)
    {
        ReturnToIdle();
    }
}

void AnimationReactionSystem::TriggerTaunt()
{
    LOG("TriggerTaunt called.");

    if (IsPlayingReaction())
    {
        LOG("TriggerTaunt ignored because another reaction is playing.");
        return;
    }

    PlayReaction(
        Action::Taunt,
        mPlayerAnimations.taunt,
        mNpcAnimations.attack,
        5.0f);
}

void AnimationReactionSystem::TriggerGreet()
{
    LOG("TriggerGreet called.");

    if (IsPlayingReaction())
    {
        LOG("TriggerGreet ignored because another reaction is playing.");
        return;
    }

    PlayReaction(
        Action::Greet,
        mPlayerAnimations.greet,
        mNpcAnimations.greet,
        5.0f);
}

void AnimationReactionSystem::PlayReaction(
    Action action,
    int playerAnimation,
    int npcAnimation,
    float duration)
{
    if (playerAnimation < 0 || npcAnimation < 0)
    {
        LOG("AnimationReactionSystem: Invalid animation index.");
        return;
    }

    mCurrentAction = action;
    mReactionTimer = 0.0f;
    mReactionDuration = duration;

    const bool playerPlayed =
        mPlayerAnimator->Play(playerAnimation, false);

    const bool npcPlayed =
        mNpcAnimator->Play(npcAnimation, false);

    LOG("Player Play(%d) result = %s",
        playerAnimation,
        playerPlayed ? "true" : "false");

    LOG("NPC Play(%d) result = %s",
        npcAnimation,
        npcPlayed ? "true" : "false");

    if (!playerPlayed || !npcPlayed)
    {
        LOG("AnimationReactionSystem: One or both animation indices are invalid.");
    }
}

void AnimationReactionSystem::ReturnToIdle()
{
    mCurrentAction = Action::None;
    mReactionTimer = 0.0f;
    mReactionDuration = 0.0f;

    if (mPlayerAnimator != nullptr)
    {
        const bool playerIdlePlayed =
            mPlayerAnimator->Play(mPlayerAnimations.idle, true);

        LOG("Player Idle Play(%d) result = %s",
            mPlayerAnimations.idle,
            playerIdlePlayed ? "true" : "false");
    }

    if (mNpcAnimator != nullptr)
    {
        const bool npcIdlePlayed =
            mNpcAnimator->Play(mNpcAnimations.idle, true);

        LOG("NPC Idle Play(%d) result = %s",
            mNpcAnimations.idle,
            npcIdlePlayed ? "true" : "false");
    }
}