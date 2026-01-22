#include "Precompiled.h"
#include "ShadowEffect.h"

#include "RenderObject.h"
#include "VertexTypes.h"

using namespace VEngine;
using namespace VEngine::Graphics;


void VEngine::Graphics::ShadowEffect::Initialize()
{
	std::filesystem::path shaderFile = L"../../Assets/Shaders/Shadow.fx";
	mVertexShader.Initialize<Vertex>(shaderFile);
	mPixelShader.Initialize(shaderFile);
	mTransformBuffer.Initialize();

	mLightCamera.SetMode(Camera::ProjectionMode::Orthographic);
	mLightCamera.SetNearPlane(1.0f);
	mLightCamera.SetFarPlane(2000.0f);

	constexpr uint32_t depthMapResolution = 4096;
	mDepthMapRenderTarget.Initialize(depthMapResolution, depthMapResolution, RenderTarget::Format::RGBA_U32);

}

void VEngine::Graphics::ShadowEffect::Terminate()
{
	mDepthMapRenderTarget.Terminate();
	mTransformBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void VEngine::Graphics::ShadowEffect::Begin()
{
	UpdateLightCamera();

	mVertexShader.Bind();
	mPixelShader.Bind();
	mTransformBuffer.BindVS(0);

	mDepthMapRenderTarget.BeginRender();
}

void VEngine::Graphics::ShadowEffect::End()
{
	mDepthMapRenderTarget.EndRender();
}

void VEngine::Graphics::ShadowEffect::Render(const RenderObject& renderObject)
{
	const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
	const Math::Matrix4 matView = mLightCamera.GetViewMatrix();
	const Math::Matrix4 matProj = mLightCamera.GetProjectionMatrix();

	TransformData data;
	data.wvp = Math::Transpose(matWorld * matView * matProj);
	mTransformBuffer.Update(data);

	renderObject.meshBuffer.Render();
}

void VEngine::Graphics::ShadowEffect::Render(const RenderGroup& renderGroup)
{
	const Math::Matrix4 matWorld = renderGroup.transform.GetMatrix4();
	const Math::Matrix4 matView = mLightCamera.GetViewMatrix();
	const Math::Matrix4 matProj = mLightCamera.GetProjectionMatrix();

	TransformData data;
	data.wvp = Math::Transpose(matWorld * matView * matProj);
	mTransformBuffer.Update(data);
	for (const RenderObject& renderObject : renderGroup.renderObjects)
	{
		renderObject.meshBuffer.Render();
	}
}

void VEngine::Graphics::ShadowEffect::DebugUI()
{
	if (ImGui::CollapsingHeader("ShadowEffect", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("DepthMap");
		ImGui::Image(
			mDepthMapRenderTarget.GetRawData(),
			{ 144, 144 },
			{ 0, 0 },
			{ 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 });
		ImGui::DragFloat("Size##ShadowEffect", &mSize, 1.0f, 1.0f, 1000.0f);
		ImGui::DragFloat3("FocusPoint##ShadowEffect", &mFocusPoint.x);
	}
}

void VEngine::Graphics::ShadowEffect::SetDirectionalLight(const DirectionalLight& directionalLight)
{
	mDirectionalLight = &directionalLight;
}

void VEngine::Graphics::ShadowEffect::SetFocus(const Math::Vector3& focusPoint)
{
	mFocusPoint = focusPoint;
}

void VEngine::Graphics::ShadowEffect::SetSize(float size)
{
	mSize = size;
}

const Camera& VEngine::Graphics::ShadowEffect::GetLightCamera() const
{
	return mLightCamera;
}

const Texture& VEngine::Graphics::ShadowEffect::GetDepthMap() const
{
	return mDepthMapRenderTarget;
}

void VEngine::Graphics::ShadowEffect::UpdateLightCamera()
{
	ASSERT(mDirectionalLight != nullptr, "ShadowEffect: No Light set");
	const Math::Vector3& direction = mDirectionalLight->direction;
	mLightCamera.SetDirection(direction);
	mLightCamera.SetPosition(mFocusPoint - (direction * 1000.0f));
	mLightCamera.SetSize(mSize, mSize);
}
