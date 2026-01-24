#include "Precompiled.h"
#include "Animator.h"
#include "Animation.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void VEngine::Graphics::Animator::Initialize(ModelId id)
{
	//Store model Id
	mModelId = id;
	mIsLooping = false;
	mAnimationTick = 0.0f;
	//-1 means not animating
	mClipIndex = -1;

}

//Reset animation, play new one
void VEngine::Graphics::Animator::PlayAnimation(int clipIndex, bool looping)
{
	mClipIndex = clipIndex;
	mIsLooping = looping;
	mAnimationTick = 0.0f;
}

void VEngine::Graphics::Animator::Update(float deltaTime)
{
	//Not playing so dont update
	if (mClipIndex < 0)
	{
		return;
	}

	const Model* model = ModelManager::Get()->GetModel(mModelId);
	const AnimationClip& animClip = model->animationClips[mClipIndex];
	mAnimationTick += animClip.ticksPerSecond * deltaTime;
	if (mIsLooping)
	{
		while (mAnimationTick > animClip.tickDuration)
		{
			mAnimationTick -= animClip.tickDuration;
		}
	}
	else
	{
		//If not looping, clamp the end of duration
		mAnimationTick = Math::Min(mAnimationTick, animClip.tickDuration);
	}
}

bool VEngine::Graphics::Animator::IsFinished() const
{
	if (mClipIndex < 0 || mIsLooping)
	{
		return false;
	}

	const Model* model = ModelManager::Get()->GetModel(mModelId);
	const AnimationClip& animClip = model->animationClips[mClipIndex];
	return mAnimationTick >= animClip.tickDuration;
}

size_t VEngine::Graphics::Animator::GetAnimationCount() const
{
	const Model* model = ModelManager::Get()->GetModel(mModelId);
	return model->animationClips.size();
}

bool VEngine::Graphics::Animator::GetToParentTransform(const Bone* bone, Math::Matrix4& transform) const
{
	if (mClipIndex < 0)
	{
		return false;
	}

	const Model* model = ModelManager::Get()->GetModel(mModelId);
	const AnimationClip& animClip = model->animationClips[mClipIndex];
	const Animation* animation = animClip.boneAnimations[bone->index].get();
	if (animation == nullptr)
	{
		return false;
	}
	Transform animTransform = animation->GetTransform(mAnimationTick);
	transform = animTransform.GetMatrix4();
	return true;
}
