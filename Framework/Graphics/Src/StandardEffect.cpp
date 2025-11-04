#include "Precompiled.h"
#include "StandardEffect.h"

#include "Camera.h"
#include "VertexTypes.h"
#include "RenderObject.h"
#include "TextureManager.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void StandardEffect::Initialize(const std::filesystem::path& path)
{
	// buffers
	mTransformBuffer.Initialize();
	mDirectionalLightBuffer.Initialize();
	mMaterialBuffer.Initialize();
	mSettingsBuffer.Initialize();

	// other stuff
	mVertexShader.Initialize<Vertex>(path);
	mPixelShader.Initialize(path);
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}
void StandardEffect::Terminate()
{
	mSampler.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mSettingsBuffer.Terminate();
	mMaterialBuffer.Terminate();
	mDirectionalLightBuffer.Terminate();
	mTransformBuffer.Terminate();
}
void StandardEffect::Begin()
{
	mVertexShader.Bind();
	mPixelShader.Bind();
	mSampler.BindVS(0);
	mSampler.BindPS(0);

	mTransformBuffer.BindVS(0);
	mDirectionalLightBuffer.BindVS(1);
	mDirectionalLightBuffer.BindPS(1);
	mMaterialBuffer.BindPS(2);
	mSettingsBuffer.BindVS(3);
	mSettingsBuffer.BindPS(3);
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

	TransformData data;
	data.wvp = Math::Transpose(matFinal);
	data.world = Math::Transpose(matWorld);
	data.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(data);

	SettingsData settings;
	settings.useDiffuseMap = (renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
	settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
	settings.useNormalMap = (renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
	settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
	settings.bumpWeight = mSettingsData.bumpWeight;
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
void StandardEffect::Render(const RenderGroup& renderGroup)
{
	const Math::Matrix4 matWorld = renderGroup.transform.GetMatrix4();
	const Math::Matrix4 matView = mCamera->GetViewMatrix();
	const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
	const Math::Matrix4 matFinal = matWorld * matView * matProj;

	TransformData data;
	data.wvp = Math::Transpose(matFinal);
	data.world = Math::Transpose(matWorld);
	data.viewPosition = mCamera->GetPosition();
	mTransformBuffer.Update(data);

	mDirectionalLightBuffer.Update(*mDirectionalLight);

	TextureManager* tm = TextureManager::Get();
	SettingsData settings;
	for (const RenderObject& renderObject : renderGroup.renderObjects)
	{
		settings.useDiffuseMap = (renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
		settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
		settings.useNormalMap = (renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
		settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
		settings.bumpWeight = mSettingsData.bumpWeight;
		mSettingsBuffer.Update(settings);
		mMaterialBuffer.Update(renderObject.material);

		tm->BindPS(renderObject.diffuseMapId, 0);
		tm->BindPS(renderObject.specMapId, 1);
		tm->BindPS(renderObject.normalMapId, 2);
		tm->BindPS(renderObject.bumpMapId, 3);

		renderObject.meshBuffer.Render();
	}
}
void StandardEffect::SetCamera(const Camera& camera)
{
	mCamera = &camera;
}
void StandardEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
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
		ImGui::DragFloat("BumpWeight", &mSettingsData.bumpWeight, 0.01f, 0.0f, 100.0f);
	}
}