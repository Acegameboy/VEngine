#include "Precompiled.h"
#include "TPSCameraComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "SaveUtil.h"

using namespace VEngine;

void VEngine::TPSCameraComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	mCameraComponent = GetOwner().GetComponent<CameraComponent>();
	ASSERT(mTransformComponent != nullptr && mCameraComponent == nullptr,
		"TPSCameraComponent: is missing either transform or camera");
}

void VEngine::TPSCameraComponent::LateUpdate(float deltaTime)
{
	Graphics::Camera& camera = mCameraComponent->GetCamera();
	Math::Matrix4 matWorld = mTransformComponent->GetMatrix4();
	Math::Vector3 targetPosition = Math::TransformCoord(mOffset, mTransformComponent->GetMatrix4());
	targetPosition = Math::Lerp(camera.GetPosition(), targetPosition, deltaTime * mSmoothingValue);
	camera.SetPosition(targetPosition);
	camera.SetDirection(Math::GetLook(matWorld));
}

void VEngine::TPSCameraComponent::DebugUI()
{
	ImGui::DragFloat3("OffSet", &mOffset.x, 0.1f);
	ImGui::DragFloat("Smoothing", &mSmoothingValue, 0.1f, 0.1f, 100.0f);
}

void VEngine::TPSCameraComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadVector3("Offset", mOffset, value);
	SaveUtil::ReadFloat("Smoothing", mSmoothingValue, value);
}
