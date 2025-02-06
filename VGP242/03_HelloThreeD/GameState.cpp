#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	std::filesystem::path shaderFile = L"../../Assets/Shaders/DoColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderFile);
	mPixelShader.Initialize(shaderFile);

	//mMeshBuffer
}
void GameState::Terminate()
{
	mMeshBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
}
void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}
void GameState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	//mMeshBuffer.Render();
}
void GameState::UpdateCamera(float deltaTime)
{
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.f : 1.0f;
	const float turnSpeed = 0.1f;

	if (input->IsKeyDown(KeyCode::W))
	{
		mCamera.Walk(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::S))
	{
		mCamera.Walk(-moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::D))
	{
		mCamera.Strafe(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::A))
	{
		mCamera.Strafe(-moveSpeed * deltaTime);
	}

	if (input->IsKeyDown(KeyCode::E))
	{
		mCamera.Rise(moveSpeed * deltaTime);
	}
	else if (input->IsKeyDown(KeyCode::Q))
	{
		mCamera.Rise(-moveSpeed * deltaTime);
	}


	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}