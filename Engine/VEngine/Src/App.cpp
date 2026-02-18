#include"Precompiled.h"
#include "App.h"
#include "AppState.h"

using namespace VEngine;
using namespace VEngine::Core;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Physics;

void App::Run(const AppConfig& config)
{
	LOG("App started: %.3f", TimeUtil::GetTime());

	Window myWindow;
	myWindow.Initialize(
		GetModuleHandle(nullptr),
		config.appName,
		config.winWidth,
		config.winHeight);
	ASSERT(myWindow.IsActive(), "App: Failed to create window");

	//call all static initializes
	HWND window = myWindow.GetWindowHandle();
	GraphicsSystem::StaticInitials(window, false);
	InputSystem::StaticInitialize(window);
	DebugUI::StaticInitialize(window, false, true);
	SimpleDraw::StaticInitialize(config.maxVertexCount);
	TextureManager::StaticInitialize(L"../../Assets/Textures");
	ModelManager::StaticInitialize(L"../../Assets/Models");

	PhysicsWorld::Settings settings;
	PhysicsWorld::StaticInitialize(settings);

	//Initialize current state
	ASSERT(mCurrentState != nullptr, "App: need an app state to start");
	mCurrentState->Initialize();

	mRunning = true;
	while (mRunning)
	{
		//Run the application
		myWindow.ProcessMessage();

		InputSystem* inputSystem = InputSystem::Get();
		inputSystem->Update();

		if (!myWindow.IsActive() || inputSystem->IsKeyPressed(KeyCode::ESCAPE))
		{
			Quit();
			break;
		}

		if (mNextState != nullptr)
		{
			mCurrentState->Terminate();
			mCurrentState = std::exchange(mNextState, nullptr);
			mCurrentState->Initialize();
		}

		float deltaTime = TimeUtil::GetDeltaTime();
#if defined(_DEBUG)
		if (deltaTime < 0.5f) //Primarily for hadling breakpoints
#endif
		{
			mCurrentState->Update(deltaTime);
			PhysicsWorld::Get()->Update(deltaTime);
		}

		//render everything
		GraphicsSystem* gs = GraphicsSystem::Get();
		gs->BeginRender();
			mCurrentState->Render();
			DebugUI::BeginRender();
			mCurrentState->DebugUI();
			DebugUI::EndRender();
		gs->EndRender();
	}

	//terminate current state
	mCurrentState->Terminate();

	//call all static terminates
	PhysicsWorld::StaticTerminate();
	TextureManager::StaticTerminate();
	ModelManager::StaticTerminate();
	SimpleDraw::StaticTerminate();
	DebugUI::StaticTerminate();
	InputSystem::StaticTerminate();
	GraphicsSystem::StaticTerminate();


	//destroy window
	myWindow.Terminate();
}

void App::Quit()
{
	bool mRunning = false;
}

void App::ChangeState(const std::string& stateName)
{
	auto iter = mAppStates.find(stateName);
	if (iter != mAppStates.end())
	{
		mNextState = iter->second.get();
	}
}
