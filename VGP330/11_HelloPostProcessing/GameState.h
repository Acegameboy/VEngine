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
	VEngine::Graphics::RenderGroup mCharacter02;
	VEngine::Graphics::RenderGroup mCharacter03;
	VEngine::Graphics::StandardEffect mStandardEffect;
	VEngine::Graphics::PostProcessingEffect mPostProcessingEffect;

	VEngine::Graphics::RenderTarget mRenderTarget;

	VEngine::Graphics::RenderObject mGround;
	VEngine::Graphics::RenderObject mScreenQuad;

	VEngine::Graphics::Texture mCombineTexture;

	int mCurrentSelection = -1;
};