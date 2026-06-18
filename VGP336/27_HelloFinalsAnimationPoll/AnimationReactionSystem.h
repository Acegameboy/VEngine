#pragma once

#include <VEngine/Inc/VEngine.h>


class AnimationReactionSystem
{
public:
    enum class Action
    {
        None,
        Taunt,
        Greet,
        Dance
    };

    struct AnimationPool
    {
        int idle = 0;

        std::vector<int> greetAnimations;
        std::vector<int> tauntAnimations;
        std::vector<int> attackAnimations;
        std::vector<int> danceAnimations;
        std::vector<int> threatAnimations;
    };

    void Initialize(
        VEngine::GameObject* player,
        VEngine::GameObject* npc);

    void Update(float deltaTime);

    void TriggerTaunt();
    void TriggerGreet();
    void TriggerDance();

    bool IsPlayingReaction() const
    {
        return mCurrentAction != Action::None;
    }

private:
    int PickRandomAnimation(const std::vector<int>& animationList);

    bool IsValidAnimationIndex(
        VEngine::AnimatorComponent* animator,
        int animationIndex) const;

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

    std::random_device mRandomDevice;
    std::mt19937 mRandomGenerator{ mRandomDevice() };
};
