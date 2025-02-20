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
	VEngine::Graphics::MeshBuffer mMeshBuffer;
	VEngine::Graphics::VertexShader mVertexShader;
	VEngine::Graphics::PixelShader mPixelShader;
	VEngine::Graphics::ConstantBuffer mTransformBuffer;
	VEngine::Graphics::Texture mTexture;
	VEngine::Graphics::Sampler mSampler;
	VEngine::Math::Vector3 mPosition;
	VEngine::Math::Vector3 mLineA;
	VEngine::Math::Vector3 mLineB;
	VEngine::Color mLineColor;

	bool mShowAABB = false;
	bool mShowFilledAABB = false;
	bool mShowSphere = false;
	bool mShowGroundPlane = false;
	bool mShowGroundCircle = false;
	bool mShowTransform = false;
};