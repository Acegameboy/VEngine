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
	VEngine::Graphics::Camera mRenderTargetCamera;
	VEngine::Graphics::DirectionalLight mDirectionalLight;
	VEngine::Graphics::RenderObject mRenderObject;
	VEngine::Graphics::StandardEffect mStandardEffect;

	VEngine::Graphics::RenderTarget mRenderTarget;

	VEngine::Graphics::PointLight mSunLight;

	int mCurrentSelection = -1;
	bool showOrbitRings = true;
	bool mShowGrid = false;
	int mFocusedPlanetIndex = 0;

};