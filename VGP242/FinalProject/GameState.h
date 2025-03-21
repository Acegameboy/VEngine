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
	VEngine::Graphics::PointLight mPointLight;
	VEngine::Graphics::RenderObject mRenderObject;
	VEngine::Graphics::StandardEffect mStandardEffect;

	VEngine::Graphics::RenderTarget mRenderTarget;

	int mCurrentSelection = 0;
	bool showOrbitRings = true;
	bool mShowGrid = false;
	int mFocusedPlanetIndex = 0;
	bool mWireframeMode = false;
};