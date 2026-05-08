#pragma once

#include "Component.h"

namespace VEngine
{
	class SoundEffectComponent : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::SoundEffect);

		void Initialize() override;
		void Terminate() override;
		void DebugUI() override;
		void Deserialize(const rapidjson::Value& value) override;

		void Play();
		void Stop();
	private:
		std::string mFileName;
		bool mLooping = false;

	};

}
