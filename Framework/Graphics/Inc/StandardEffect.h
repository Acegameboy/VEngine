#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "Material.h"
#include "LightTypes.h"

namespace VEngine::Graphics
{
	class Camera;
	struct RenderObject;

	class StandardEffect
	{
	public:
		void Initialize(const std::filesystem::path& filePath);
		void Terminate();

		void Begin();
		void End();
		void Render(const RenderObject& renderObject);
		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetPointLight(const PointLight& pointLight);
		void DebugUI();

	private:
		struct TransformData
		{
			Math::Matrix4 wvp;
			Math::Matrix4 world;
			Math::Vector3 viewPosition;
			float padding = 0.0f;
		};

		struct SettingsData
		{
			int useTexture = 1;
			int useLighting = 1;
			int padding[2] = { 0 };
		};

		using TransformBuffer = TypeConstantBuffer<TransformData>;
		using SettingsBuffer = TypeConstantBuffer<SettingsData>;
		using MaterialBuffer = TypeConstantBuffer<Material>;
		using DirectionalLightBuffer = TypeConstantBuffer<DirectionalLight>;
		using PointLightBuffer = TypeConstantBuffer<PointLight>;
		TransformBuffer mTransformBuffer;
		SettingsBuffer mSettingsBuffer;
		MaterialBuffer mMaterialBuffer;
		DirectionalLightBuffer mDirectionalLightBuffer;
		PointLightBuffer mPointLightBuffer;

		VEngine::Graphics::VertexShader mVertexShader;
		VEngine::Graphics::PixelShader mPixelShader;
		VEngine::Graphics::Sampler mSampler;

		const Camera* mCamera = nullptr;
		SettingsData mSettingsData;
		const DirectionalLight* mDirectionalLight = nullptr;
		const PointLight* mPointLight = nullptr;
	};
}
