#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "BlendState.h"
#include "Colors.h"
#include "MeshBuffer.h"
#include "TextureManager.h"

namespace VEngine::Graphics
{
	class Camera;
	struct Transform;

	class ParticleSystemEffect final
	{
	public:
		void Initialize();
		void Terminate();

		void Begin();
		void End();

		void Render(const Transform& transform, const Color& color);

		void DebugUI();
		void SetCamera(const Camera& camera);
		void SetTextureID(TextureId id);

	private:
		using ParticleBuffer = TypeConstantBuffer<Math::Matrix4>;
		using ColorBuffer = TypeConstantBuffer<Color>;
		ParticleBuffer mParticleBuffer;
		ColorBuffer mColorBuffer;

		VertexShader mVertexShader;
		PixelShader mPixelShader;
		Sampler mSampler;
		BlendState mBlendState;

		MeshBuffer mParticle;

		TextureId mTextureId = 0;
		const Camera* mCamera = nullptr;
	};
}
