#include <VEngine/Inc/VEngine.h>
#include "ShapeStates.h"

using namespace VEngine;
using namespace VEngine::Graphics;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	AppConfig config;
	config.appName = L"Hello Shapes";

	App& myApp = MainApp();
	myApp.AddState<ShapeState>("ShapeState");
	myApp.AddState<ColoredShapeState>("ColoredShapeState");
	myApp.Run(config);

	return 0;
}