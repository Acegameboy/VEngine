#include"Precompiled.h"
#include "App.h"
#include "AppState.h"

using namespace VEngine;
using namespace VEngine::Core;
using namespace VEngine::Graphics;

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

	ASSERT(mCurrentState != nullptr, "App: need an app state to start");
	mCurrentState->Initialize();

	mRunning = true;
	while (mRunning)
	{
		//Run the application
		myWindow.ProcessMessage();
		if (!myWindow.IsActive())
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

		//update current state
		float deltaTime = TimeUtil::GetDeltaTime();
		mCurrentState->Update(deltaTime);

		//render everything
		GraphicsSystem* gs = GraphicsSystem::Get();
		gs->BeginRender();
			mCurrentState->Render();
		gs->EndRender();
	}

	//terminate current state
	mCurrentState->Terminate();

	//call all static terminates
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
