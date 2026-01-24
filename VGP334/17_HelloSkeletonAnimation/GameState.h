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
	VEngine::Graphics::RenderGroup mCharacter01;
	VEngine::Graphics::Animator mAnimator;

	VEngine::Graphics::StandardEffect mStandardEffect;

	VEngine::Graphics::RenderTarget mRenderTarget;

	float mAnimationSpeed = 1.0f;
	int mClipIndex = -1;

	int mCurrentSelection = -1;

	bool mDrawSkeleton = false;
};