#pragma once

#include "CustomTypeIds.h"
#include <VEngine/Inc/VEngine.h>

class CustomDebugDrawComponent;

class CustomDebugDrawService : public VEngine::Service
{
public:
	SET_TYPE_ID(CustomServiseId::CustomDebugDrawDisplay);

	void Render() override;

	void Register(const CustomDebugDrawComponent* debugDrawComponent);

	void Unregister(const CustomDebugDrawComponent* debugDrawComponent);

};