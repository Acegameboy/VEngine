#include "Precompiled.h"
#include "StandardEffect.h"

#include "Camera.h"
#include "VertexTypes.h"
#include "RenderObject.h"

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
}

void StandardEffect::Terminate()
{
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
	mDirectionalLightBuffer.BindPS(3);
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
	mSettingsBuffer.Update(mSettingsData);
	mMaterialBuffer.Update(renderObject.material);
	mDirectionalLightBuffer.Update(*mDirectionalLight);

	renderObject.texture.BindPS(0);
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

void StandardEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("StandardEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool useTexture = mSettingsData.useTexture > 0;
		if (ImGui::Checkbox("UseTexture", &useTexture))
		{
			mSettingsData.useTexture = useTexture ? 1 : 0;
		}
		bool useLighting = mSettingsData.useLighting > 0;
		if (ImGui::Checkbox("UseLighting", &useLighting))
		{
			mSettingsData.useLighting = useLighting ? 1 : 0;
		}
	}
}
