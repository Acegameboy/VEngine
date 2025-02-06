#include <VEngine/Inc/VEngine.h>

using namespace VEngine;
using namespace VEngine::Graphics;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	AppConfig config;
	config.appName = L"Hello Shapes";

	App& myApp = MainApp();
	myApp.AddState<ShapeState>("ShapeState");
	myApp.AddState<ColoredShapeState>("ColoredShapeState");
	myApp.AddState<TriforceShapeState>("TriforceShapeState");
	myApp.AddState<DiamondShapeState>("DiamondShapeState");
	myApp.AddState<HeartShapeState>("HeartShapeState");
	myApp.Run(config);

	return 0;
}