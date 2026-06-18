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

    //Character01
    mPlayerAnimations.idle = 0;

    mPlayerAnimations.tauntAnimations = {
        1, //taunt 1
        2, //taunt 2
        3  //taunt 3
    };

    mPlayerAnimations.greetAnimations = {
        4, //greet 1
        5, //greet 2
        6  //greet 3
    };

    mPlayerAnimations.danceAnimations = {
        7, //dance 1
        8, //dance 2
        9  //dance 3
    };

    //Character02
    mNpcAnimations.idle = 0;

    mNpcAnimations.threatAnimations = {
        1,
        2,
        3
    };

    mNpcAnimations.greetAnimations = {
        4,
        5,
        6
    };

    mNpcAnimations.danceAnimations = {
        7, //greet 1
        8, //greet 2
        9  //greet 3
    };

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

    const int playerTaunt =
        PickRandomAnimation(mPlayerAnimations.tauntAnimations);

    const int npcAttack =
        PickRandomAnimation(mNpcAnimations.threatAnimations);

    PlayReaction(
        Action::Taunt,
        playerTaunt,
        npcAttack,
        3.0f);
}

void AnimationReactionSystem::TriggerGreet()
{
    LOG("TriggerGreet called.");

    if (IsPlayingReaction())
    {
        LOG("TriggerGreet ignored because another reaction is playing.");
        return;
    }

    const int playerGreet =
        PickRandomAnimation(mPlayerAnimations.greetAnimations);

    const int npcGreet =
        PickRandomAnimation(mNpcAnimations.greetAnimations);

    PlayReaction(
        Action::Greet,
        playerGreet,
        npcGreet,
        3.0f);
}

void AnimationReactionSystem::TriggerDance()
{
    if (IsPlayingReaction())
    {
        LOG("TriggerDance ignored because another reaction is playing.");
        return;
    }

    const int playerDance =
        PickRandomAnimation(mPlayerAnimations.danceAnimations);

    const int npcDance =
        PickRandomAnimation(mNpcAnimations.danceAnimations);

    PlayReaction(
        Action::Dance,
        playerDance,
        npcDance,
        3.0f);
}

int AnimationReactionSystem::PickRandomAnimation(
    const std::vector<int>& animationList)
{
    if (animationList.empty())
    {
        return -1;
    }

    std::uniform_int_distribution<int> distribution(
        0,
        static_cast<int>(animationList.size()) - 1);

    const int randomIndex = distribution(mRandomGenerator);

    return animationList[randomIndex];
}

bool AnimationReactionSystem::IsValidAnimationIndex(
    AnimatorComponent* animator,
    int animationIndex) const
{
    if (animator == nullptr)
    {
        return false;
    }

    if (animationIndex < 0)
    {
        return false;
    }

    const uint32_t animationCount =
        animator->GetAnimator().GetAnimationCount();

    return animationIndex < static_cast<int>(animationCount);
}

void AnimationReactionSystem::PlayReaction(
    Action action,
    int playerAnimation,
    int npcAnimation,
    float duration)
{
    if (!IsValidAnimationIndex(mPlayerAnimator, playerAnimation))
    {
        LOG("Player animation index %d is invalid.", playerAnimation);
        return;
    }

    if (!IsValidAnimationIndex(mNpcAnimator, npcAnimation))
    {
        LOG("NPC animation index %d is invalid.", npcAnimation);
        return;
    }

    mCurrentAction = action;
    mReactionTimer = 0.0f;
    mReactionDuration = duration;

    const bool playerPlayed =
        mPlayerAnimator->Play(playerAnimation, false);

    const bool npcPlayed =
        mNpcAnimator->Play(npcAnimation, false);

    LOG("Player randomly picked animation %d. Play result = %s",
        playerAnimation,
        playerPlayed ? "true" : "false");

    LOG("NPC randomly picked animation %d. Play result = %s",
        npcAnimation,
        npcPlayed ? "true" : "false");
}

void AnimationReactionSystem::ReturnToIdle()
{
    mCurrentAction = Action::None;
    mReactionTimer = 0.0f;
    mReactionDuration = 0.0f;

    if (IsValidAnimationIndex(mPlayerAnimator, mPlayerAnimations.idle))
    {
        mPlayerAnimator->Play(mPlayerAnimations.idle, true);
    }

    if (IsValidAnimationIndex(mNpcAnimator, mNpcAnimations.idle))
    {
        mNpcAnimator->Play(mNpcAnimations.idle, true);
    }
}