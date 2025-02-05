#pragma once

#include <VEngine/Inc/VEngine.h>

class ShapeState : public VEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;

protected:
	VEngine::Graphics::VertexShader mVertexShader;
	VEngine::Graphics::PixelShader mPixelShader;
	VEngine::Graphics::MeshBuffer mMeshBuffer;
};

class ColoredShapeState : public ShapeState
{
public:
	void Initialize() override;
	void Update(float deltaTime) override;
};
