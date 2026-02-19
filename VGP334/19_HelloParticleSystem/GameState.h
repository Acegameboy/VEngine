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

	void UpdateCamera(float deltaTime);

	VEngine::Graphics::Camera mCamera;
	VEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;
	VEngine::Physics::ParticleSystem mParticleSystem;
};