#include "GameState.h"
#include "CustomDebugDrawComponent.h"
#include "CustomDebugDrawService.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Physics;

Service* MakeCustomService(const std::string& serviceName, GameWorld& gameWorld)
{
	if (serviceName == "CustomDebugDrawService")
	{
		return gameWorld.AddService<CustomDebugDrawService>();
	}
	//add anoither
	//add next...
	return nullptr;
}

Component* MakeCustomComponent(const std::string& componentName, GameObject& gameObject)
{
	if (componentName == "CustomDebugDrawComponent")
	{
		return gameObject.AddComponent<CustomDebugDrawComponent>();
	}
	return nullptr;
}

Component* GetCustomComponent(const std::string& componentName, GameObject& gameObject)
{
	if (componentName == "CustomDebugDrawComponent")
	{
		return gameObject.GetComponent<CustomDebugDrawComponent>();
	}
	return nullptr;
}

void GameState::Initialize()
{
	mLevelFile = L"../../Assets/Templates/Levels/level.json";
	
	//Set a callback to try make a custom service(any service that is NOT part of the engine and unique to the project)
	GameWorld::SetCustomService(MakeCustomService);
	//Set callbacks to try make/get a custom component (any component that is NOT part of the engine)
	GameObjectFactory::SetCustomMake(MakeCustomComponent);
	GameObjectFactory::SetCustomGet(GetCustomComponent);

	mGameWorld.LoadLevel(mLevelFile);

}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	mGameWorld.DebugUI();
	if (ImGui::Button("ReloadLevel"))
	{
		mGameWorld.Terminate();
		mGameWorld.LoadLevel(mLevelFile);
	}
	ImGui::End();
}