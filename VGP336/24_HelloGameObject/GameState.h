#pragma once

#include <VEngine/Inc/VEngine.h>

class GameState : public VEngine::AppState
{
public:
	void Initialize() override;

	void Terminate() override;

	void Update(float deltaTime) override;

	void Render() override;

	void DebugUI() override;

private:
	VEngine::GameWorld mGameWorld;
	VEngine::CameraComponent* mCameraComponent = nullptr;
	
};