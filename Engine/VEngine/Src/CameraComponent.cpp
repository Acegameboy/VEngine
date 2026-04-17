#include "Precompiled.h"
#include "CameraComponent.h"

using namespace VEngine;

void VEngine::CameraComponent::Initialize()
{

}

void VEngine::CameraComponent::Terminate()
{

}

void VEngine::CameraComponent::DebugUI()
{
	Math::Vector3 pos = mCamera.GetPosition();
	Math::Vector3 dir = mCamera.GetDirection();
	if (ImGui::DragFloat3("Position##Camera", &pos.x, 0.1f))
	{
		mCamera.SetPosition(dir);
	}
	if (ImGui::DragFloat3("Direction##Camera", &dir.x, 0.1f))
	{
		mCamera.SetDirection(dir);
	}
}

Graphics::Camera& VEngine::CameraComponent::GetCamera()
{
	return mCamera;
}

const Graphics::Camera& VEngine::CameraComponent::GetCamera() const
{
	return mCamera;
}
