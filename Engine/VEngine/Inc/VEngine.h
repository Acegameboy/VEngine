#pragma once

#include "Common.h"

#include "App.h"
#include "AppState.h"
#include "SaveUtil.h"

//GameObject info
#include "GameObject.h"
#include "GameWorld.h"
#include "GameObjectHandle.h"
#include "GameObjectFactory.h"

//Component
#include "TypeId.h"
#include "Component.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"
#include "RenderObjectComponent.h"
#include "MeshCompoent.h"
#include "ModelComponent.h"
#include "AnimatorComponent.h"
#include "RigidBodyComponent.h"
#include "SoundEventComponent.h"
#include "SoundBankComponent.h"


//Service
#include "Service.h"
#include "CameraService.h"
#include "RenderService.h"
#include "PhysicsService.h"

namespace VEngine
{
	App& MainApp();
}