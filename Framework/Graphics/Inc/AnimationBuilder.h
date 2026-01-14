#pragma once

#include "AnimationBuilder.h"

namespace VEngine::Graphics
{
	class AnimationBuilder
	{
	public:
		AnimationBuilder& AddPositionKeys(const Math::Vector3& pos, float time);
		AnimationBuilder& AddRotationKeys(const Math::Quaternion& rot, float time);
		AnimationBuilder& AddScaleKeys(const Math::Vector3& scale, float time);

		[[nodiscard]] Animation Build();
	private:
		Animation mWorkingCopy;
	};
}
