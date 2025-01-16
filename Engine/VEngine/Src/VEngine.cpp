#include "Precompiled.h"
#include "VEngine.h"

VEngine::App& VEngine::MainApp()
{
	static App sApp;
	return sApp;
}