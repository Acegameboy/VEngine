#include "Precompiled.h"
#include "Animation.h"
#include "AnimationBuilder.h"

using namespace VEngine;
using namespace VEngine::Graphics;

namespace
{
	template<class T>
	inline void PushKey(Keyframes<T>& keyframes, const T& value, float t)
	{
		ASSERT(keyframes.empty() || keyframes.back().time() <= t, "AnimationBuilder: Can't add keyframe back in time");
		keyframes.emplace_back(value, t);
	}

}

AnimationBuilder& VEngine::Graphics::AnimationBuilder::AddPositionKeys(const Math::Vector3& pos, float time)
{
	PushKey(mWorkingCopy.mPositionKeys, pos, time);
	mWorkingCopy.mDuration = Math::Max(mWorkingCopy.mDuration, time);
	return *this;
}

AnimationBuilder& VEngine::Graphics::AnimationBuilder::AddRotationKeys(const Math::Quaternion& rot, float time)
{
	PushKey(mWorkingCopy.mRotationKeys, rot, time);
	mWorkingCopy.mDuration = Math::Max(mWorkingCopy.mDuration, time);
	return *this;
}

AnimationBuilder& VEngine::Graphics::AnimationBuilder::AddScaleKeys(const Math::Vector3& scale, float time)
{
	PushKey(mWorkingCopy.mScaleKeys, scale, time);
	mWorkingCopy.mDuration = Math::Max(mWorkingCopy.mDuration, time);
	return *this;
}

Animation VEngine::Graphics::AnimationBuilder::Build()
{
	ASSERT(!mWorkingCopy.mPositionKeys.empty()
		|| !mWorkingCopy.mRotationKeys.empty()
		|| !mWorkingCopy.mScaleKeys.empty(),
		"AnimationBuilder: No animations are present");
	return std::move(mWorkingCopy);
}
