#pragma once

#include <VEngine/Inc/TypeId.h>

enum class CustomComponentId
{
	CustomDebugDraw = static_cast<int>(VEngine::ComponentId::Count)
};

enum class CustomServiceId
{
	CustomDebugDrawDisplay = static_cast<int>(VEngine::ServiceId::Count)
};
