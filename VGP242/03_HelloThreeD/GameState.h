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

private:
	void UpdateCamera(float deltaTime);

	VEngine::Graphics::Camera mCamera;
	VEngine::Graphics::MeshBuffer mMeshBuffer;
	VEngine::Graphics::VertexShader mVertexShader;
	VEngine::Graphics::PixelShader mPixelShader;

};