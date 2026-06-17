#pragma once

#include <VEngine/Inc/VEngine.h>
#include "AnimationReactionSystem.h"

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
	void UpdateAnimationInput();

	VEngine::GameWorld mGameWorld;
	VEngine::GameObject* mCameraObject = nullptr;
	VEngine::GameObject* mPlayer = nullptr;
	VEngine::GameObject* mNpc = nullptr;

	VEngine::Graphics::Camera mCamera;
	VEngine::Graphics::DirectionalLight mDirectionalLight;
	VEngine::Graphics::RenderObject mRenderObject;
	VEngine::Graphics::StandardEffect mStandardEffect;
	VEngine::Graphics::RenderGroup mCharacter01;
	VEngine::Graphics::RenderGroup mCharacter02;

	VEngine::Graphics::Animation mAnimation;
	float mAnimationTime = 0.0f;

	VEngine::GameObjectHandle mPlayerHandle;
	VEngine::GameObjectHandle mNpcHandle;

	AnimationReactionSystem mReactionSystem;
};