#include "Precompiled.h"
#include "ParticleSystemEffect.h"

#include "VertexTypes.h"
#include "Camera.h"
#include "MeshBuffer.h"
#include "Transform.h"
#include "MeshBuilder.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void VEngine::Graphics::ParticleSystemEffect::Initialize()
{
	std::filesystem::path shaderPath = L"../../Assets/Shaders/Particle.fx";
	mVertexShader.Initialize<VertexPX>(shaderPath);
	mPixelShader.Initialize(shaderPath);
	mParticleBuffer.Initialize();
	mColorBuffer.Initialize();
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
	//mBlendState.Initialize(BlendState::Mode::AlphaBlend);

	MeshPX spriteQuad = MeshBuilder::CreateSpriteQuadPX(0.5f, 0.5f);
	mParticle.Initialize(spriteQuad);
}

void VEngine::Graphics::ParticleSystemEffect::Terminate()
{
	mParticle.Terminate();
	mBlendState.Terminate();
	mSampler.Terminate();
	mColorBuffer.Terminate();
	mParticleBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void VEngine::Graphics::ParticleSystemEffect::Begin()
{
	mVertexShader.Bind();
	mPixelShader.Bind();
	mParticleBuffer.BindVS(0);
	mColorBuffer.BindPS(1);
	mSampler.BindPS(0);
	mBlendState.Set();
}

void VEngine::Graphics::ParticleSystemEffect::End()
{
	mBlendState.ClearState();
	Texture::UnBindPS(0);
}

void VEngine::Graphics::ParticleSystemEffect::Render(const Transform& transform, const Color& color)
{
	ASSERT(mTextureId != 0 && mCamera != nullptr, "ParticleSystemEffect: missing texture or camera");

	TextureManager::Get()->BindPS(mTextureId, 0);

	//Local position of the particle relative to the camera
	const Math::Vector3 localPos = Math::TransformCoord(transform.position, mCamera->GetViewMatrix());
	const Math::Matrix4 matLocalTrans = Math::Matrix4::Translation(localPos);
	const Math::Matrix4 matScale = Math::Matrix4::Scaling(transform.scale);
	const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
	const Math::Matrix4 matFinal = Transpose(matScale * matLocalTrans * matProj);
	mParticleBuffer.Update(matFinal);
	mColorBuffer.Update(color);
	mParticle.Render();
}

void VEngine::Graphics::ParticleSystemEffect::DebugUI()
{

}

void VEngine::Graphics::ParticleSystemEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void VEngine::Graphics::ParticleSystemEffect::SetTextureID(TextureId id)
{
	mTextureId = id;
}
