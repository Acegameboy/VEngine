#include "CustomDebugDrawService.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Math;

void CustomDebugDrawService::Render()
{
	for(const)
}

void CustomDebugDrawService::Register(const CustomDebugDrawComponent* debugDrawComponent)
{
	auto iter = std::find(mCustomDebugDrawComponent.begin(), mCustomDebugDrawComponent.end(), debugDrawComponent);
	if (iter == mCustomDebugDrawComponent.end())
	{
		mCustomDebugDrawComponent.push_back(debugDrawComponent);
	}
}

void CustomDebugDrawService::Unregister(const CustomDebugDrawComponent* debugDrawComponent)
{

}
