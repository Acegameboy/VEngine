#include "SoundEventComponent.h"

using namespace VEngine;
using namespace VEngine::Audio;

void VEngine::SoundEffectComponent::Initialize()
{
	ASSERT(!mFileName.empty(), "SoundEventComponent: no sound file loaded");

}

void VEngine::SoundEffectComponent::Terminate()
{
}

void VEngine::SoundEffectComponent::DebugUI()
{
}

void VEngine::SoundEffectComponent::Deserialize(const rapidjson::Value& value)
{
}

void VEngine::SoundEffectComponent::Play()
{
}

void VEngine::SoundEffectComponent::Stop()
{
}
