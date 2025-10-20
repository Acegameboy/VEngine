#include "Precompiled.h"
#include "StandardEffect.h"

#include "Camera.h"
#include "VertexTypes.h"
#include "RenderObject.h"
#include "TextureManager.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void StandardEffect::Initialize(const std::filesystem::path& filePath)
{
	mVertexShader.Initialize<Vertex>(filePath);
	mPixelShader.Initialize(filePath);
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
	mTransformBuffer.Initialize();
	mSettingsBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mDirectionalLightBuffer.Initialize();
	mPointLightBuffer.Initialize();
}

void StandardEffect::Terminate()
{
	mPointLightBuffer.Terminate();
	mDirectionalLightBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mSettingsBuffer.Terminate();
	mTransformBuffer.Terminate();
	mSampler.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void StandardEffect::Begin()
{
	ASSERT(mCamera != nullptr, "StandardEffect: No camera set!");
	mVertexShader.Bind();
	mPixelShader.Bind();

	mSampler.BindPS(0);

	mTransformBuffer.BindVS(0);
	mSettingsBuffer.BindPS(1);
	mMaterialBuffer.BindPS(2);
	mDirectionalLightBuffer.BindVS(3);
	mDirectionalLightBuffer.BindPS(3);
	mPointLightBuffer.BindVS(4);
	mPointLightBuffer.BindPS(4);
}

void StandardEffect::End()
{
}

void StandardEffect::Render(const RenderObject& renderObject)
{
	const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
	const Math::Matrix4 matView = mCamera->GetViewMatrix();
	const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
	const Math::Matrix4 matFinal = matWorld * matView * matProj;
	const Math::Matrix4 wvp = Transpose(matFinal);

	TransformData data;
	data.wvp = Transpose(matFinal);
	data.world = Transpose(matWorld);
	data.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(data);
	SettingsData settings;
	settings.useDiffuseMap =
		(renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
	settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
	settings.useNormalMap =
		(renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
	settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
	settings.bumpIntensity = mSettingsData.bumpIntensity;
	mSettingsBuffer.Update(settings);

	mDirectionalLightBuffer.Update(*mDirectionalLight);

	mMaterialBuffer.Update(renderObject.material);

	TextureManager* tm = TextureManager::Get();
	tm->BindPS(renderObject.diffuseMapId, 0);
	tm->BindPS(renderObject.specMapId, 1);
	tm->BindPS(renderObject.normalMapId, 2);
	tm->BindVS(renderObject.bumpMapId, 3);

	renderObject.meshBuffer.Render();
}

void StandardEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}

void StandardEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void VEngine::Graphics::StandardEffect::SetPointLight(const PointLight& pointLight)
{
	mPointLight = &pointLight;
}

void StandardEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("StandardEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool useDiffuseMap = mSettingsData.useDiffuseMap > 0;
		if (ImGui::Checkbox("UseDiffuseMap", &useDiffuseMap))
		{
			mSettingsData.useDiffuseMap = (useDiffuseMap) ? 1 : 0;
		}
		bool useSpecMap = mSettingsData.useSpecMap > 0;
		if (ImGui::Checkbox("UseSpecMap", &useSpecMap))
		{
			mSettingsData.useSpecMap = (useSpecMap) ? 1 : 0;
		}
		bool useNormalMap = mSettingsData.useNormalMap > 0;
		if (ImGui::Checkbox("UseNormalMap", &useNormalMap))
		{
			mSettingsData.useNormalMap = (useNormalMap) ? 1 : 0;
		}
		bool useBumpMap = mSettingsData.useBumpMap > 0;
		if (ImGui::Checkbox("UseBumpMap", &useBumpMap))
		{
			mSettingsData.useBumpMap = (useBumpMap) ? 1 : 0;
		}
		ImGui::DragFloat("BumpIntensity", &mSettingsData.bumpIntensity, 0.1f, 0.0f, 100.0f);
	}
}
