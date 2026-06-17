#pragma once

#include <VEngine/Inc/VEngine.h>

class AnimationReactionSystem
{
public:
    enum class Action
    {
        None,
        Taunt,
        Greet
    };

    struct AnimationPool
    {
        int idle = 0;
        int taunt = -1;
        int greet = -1;
        int attack = -1;
    };

    void Initialize(
        VEngine::GameObject* player,
        VEngine::GameObject* npc);

    void Update(float deltaTime);

    void TriggerTaunt();
    void TriggerGreet();

    bool IsPlayingReaction() const
    {
        return mCurrentAction != Action::None;
    }

private:
    void PlayReaction(
        Action action,
        int playerAnimation,
        int npcAnimation,
        float duration);

    void ReturnToIdle();

    VEngine::GameObject* mPlayer = nullptr;
    VEngine::GameObject* mNpc = nullptr;

    VEngine::AnimatorComponent* mPlayerAnimator = nullptr;
    VEngine::AnimatorComponent* mNpcAnimator = nullptr;

    AnimationPool mPlayerAnimations;
    AnimationPool mNpcAnimations;

    Action mCurrentAction = Action::None;

    float mReactionTimer = 0.0f;
    float mReactionDuration = 0.0f;
};
