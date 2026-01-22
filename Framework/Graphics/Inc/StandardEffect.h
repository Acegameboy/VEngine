#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "DirectionalLight.h"
#include "Sampler.h"
#include "Material.h"
#include "LightTypes.h"
#include "Camera.h"

namespace VEngine::Graphics
{
	class Camera;
	struct RenderObject;
	class RenderGroup;
	class Texture;

	class StandardEffect final
	{
	public:
		void Initialize(const std::filesystem::path& filePath);
		void Terminate();

		void Begin();
		void End();
		void Render(const RenderObject& renderObject);
		void Render(const RenderGroup& renderGroup);
		void SetCamera(const Camera& camera);
		void SetDirectionalLight(const DirectionalLight& directionalLight);
		void SetLightCamera(const Camera& camera);
		void SetShadowMap(const Texture& shadowMap);
		void DebugUI();
		//Thermal
		void SetThermalEnabled(bool enabled);
		void SetThermalBaseHeat(float value);
		void SetThermalVariation(float value);

	private:
		struct TransformData
		{
			Math::Matrix4 wvp;
			Math::Matrix4 world;
			Math::Matrix4 lwvp; // word view projection of the light object for shadows
			Math::Vector3 viewPosition;
			float padding = 0.0f;
		};

		struct SettingsData
		{
			int useDiffuseMap = 1;
			int useSpecMap = 1;
			int useNormalMap = 1;
			int useBumpMap = 1;
			int useShadowMap = 1;
			int useSkinning = 1;
			float bumpWeight = 0.1f;
			float depthBias = 0.000003f;

			//For thermal fields
			int   useThermal = 1;      // set to 1 for now
			float baseHeat = 0.5f;   // try 0.5 to see more colors
			float heatVariation = 0.5f;   // big variation to exaggerate effect
			float padding2 = 0.0f;
		};

		using TransformBuffer = TypeConstantBuffer<TransformData>;
		using SettingsBuffer = TypeConstantBuffer<SettingsData>;
		using MaterialBuffer = TypeConstantBuffer<Material>;
		using DirectionalLightBuffer = TypeConstantBuffer<DirectionalLight>;
		using PointLightBuffer = TypeConstantBuffer<PointLight>;
		using BoneTransformBuffer = ConstantBuffer;
		BoneTransformBuffer mBoneTransformBuffer;
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
		const Camera* mLightCamera = nullptr;
		const Texture* mShadowMap = nullptr;
	};
}
