#include "Precompiled.h"
#include "FPSCameraComponent.h"
#include "CameraComponent.h"
#include "GameObject.h"

using namespace VEngine;
using namespace VEngine::Input;

void VEngine::FPSCameraComponent::Initialize()
{
	mCameraComponent = GetOwner().GetComponent<CameraComponent>();
	ASSERT(mCameraComponent != nullptr, "FPSCameraComponent: camera was not found");
}

void VEngine::FPSCameraComponent::Terminate()
{
	mCameraComponent = nullptr;
}

void VEngine::FPSCameraComponent::Update(float deltaTime)
{
	Graphics::Camera& camera = mCameraComponent->GetCamera();
	auto input = InputSystem::Get();
	const float moveSpeed = ((input->IsKeyDown(KeyCode::LSHIFT)) ? mShiftSpeed : mMoveSpeed)* deltaTime;
	const float turnSpeed = mTurnSpeed * deltaTime;
	if (input->IsKeyDown(KeyCode::W))
	{
		camera.Walk(moveSpeed);
	}
	if (input->IsKeyDown(KeyCode::S))
	{
		camera.Walk(-moveSpeed);
	}
	if (input->IsKeyDown(KeyCode::D))
	{
		camera.Strafe(moveSpeed);
	}
	if (input->IsKeyDown(KeyCode::A))
	{
		camera.Strafe(-moveSpeed);
	}
	if (input->IsKeyDown(KeyCode::E))
	{
		camera.Rise(moveSpeed);
	}
	if (input->IsKeyDown(KeyCode::Q))
	{
		camera.Rise(-moveSpeed);
	}
	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		camera.Yaw(input->GetMouseMoveX() * turnSpeed);
		camera.Pitch(input->GetMouseMoveY() * turnSpeed);

	}
}

void VEngine::FPSCameraComponent::DebugUI()
{
	ImGui::DragFloat("MoveSpeed", &mMoveSpeed, 0.1f, 0.1f, 100.0f);
	ImGui::DragFloat("ShiftSpeed", &mShiftSpeed, 0.1f, 0.1f, 1000.0f);
	ImGui::DragFloat("TurnSpeed", &mTurnSpeed, 0.001f, 0.01f, 1.0f);
}
