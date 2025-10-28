#include <VEngine/Inc/VEngine.h>
#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	AppConfig config;
	config.appName = L"Hello Model";

	App& myApp = MainApp();
	myApp.AddState<GameState>("GameState");
	myApp.Run(config);

	return 0;
}