#include"Precompiled.h"
#include "App.h"
#include "AppState.h"

using namespace VEngine;
using namespace VEngine::Core;

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

		float deltaTime = TimeUtil::GetDeltaTime();
		mCurrentState->Update(deltaTime);
	}

	mCurrentState->Terminate();

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
