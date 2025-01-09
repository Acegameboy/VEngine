#include "Precompiled.h"
#include "TimeUtil.h"

float VEngine::Core::TimeUtil::GetTime()
{
	//Getting the difference between now and when the app started
	static const auto startTime = std::chrono::high_resolution_clock::now();
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

	return milliseconds / 1000.0f;
}

float VEngine::Core::TimeUtil::GetDeltaTime()
{
	//Getting the difference between now and the last time we checked
	static auto lastCallTime = std::chrono::high_resolution_clock::now();
	const auto currentTime = std::chrono::high_resolution_clock::now();
	const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastCallTime).count();
	lastCallTime = currentTime;
	return milliseconds / 1000.0f;
}
