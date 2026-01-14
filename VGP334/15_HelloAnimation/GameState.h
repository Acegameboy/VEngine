#pragma once

#include <VEngine/Inc/VEngine.h>

class GameState : public VEngine::AppState
{
public:
	~GameState() = default;

	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;
	void DebugUI() override;

private:
	void UpdateCamera(float deltaTime);


	VEngine::Graphics::Camera mCamera;
	VEngine::Graphics::DirectionalLight mDirectionalLight;
	VEngine::Graphics::RenderObject mRenderObject;
	VEngine::Graphics::StandardEffect mStandardEffect;

	VEngine::Graphics::Animation mAnimation;
	float mAnimationTime = 0.0f;
};