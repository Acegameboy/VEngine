#pragma once

#include "Common.h"

#include "App.h"
#include "AppState.h"

//GameObject info
#include "GameObject.h"
#include "GameWorld.h"
#include "GameObjectHandle.h"

//Component
#include "Component.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"

//Service
#include "Service.h"

namespace VEngine
{
	App& MainApp();
}