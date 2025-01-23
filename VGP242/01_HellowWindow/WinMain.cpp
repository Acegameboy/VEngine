#include <VEngine/Inc/VEngine.h>

using namespace VEngine;
using namespace VEngine::Graphics;

class StartState : public AppState
{
public:
	void Initialize() override
	{
		LOG("Initialized Start State");
		mLifeTime = 5.0f;

		GraphicsSystem::Get()->SetClearColor(Colors::Blue);
	}

	void Update(float deltaTime) override
	{
		mLifeTime -= deltaTime;
		if (mLifeTime <= 0.0f)
		{
			MainApp().ChangeState("GameState");
		}
	}
private:
	float mLifeTime = 0.0f;

};

class GameState : public AppState
{
public:
	void Initialize() override
	{
		LOG("Initialized Game State");
		mLifeTime = 5.0f;

		GraphicsSystem::Get()->SetClearColor(Colors::Red);
	}

	void Update(float deltaTime) override
	{
		mLifeTime -= deltaTime;
		if (mLifeTime <= 0.0f)
		{
			MainApp().ChangeState("StartState");
		}
	}
private:
	float mLifeTime = 0.0f;

};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	AppConfig config;
	config.appName = L"Hellow Window";
	App& myApp = MainApp();
	myApp.AddState<StartState>("StartState");
	myApp.AddState<GameState>("GameState");
	myApp.Run(config);

	return 0;
}