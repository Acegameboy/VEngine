#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	std::filesystem::path shaderFile = L"../../Assets/Shaders/DoTransformColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderFile);
	mPixelShader.Initialize(shaderFile);
	mTransformBuffer.Initialize(sizeof(Matrix4));

	MeshPC mesh;
	mesh.vertices.push_back({{ -0.5f, 0.0f, -0.5f }, Colors::Blue});
	mesh.vertices.push_back({ { -0.5f, 1.0f, -0.5f }, Colors::Blue });
	mesh.vertices.push_back({ { 0.5f, 1.0f, -0.5f }, Colors::Blue });
	mesh.vertices.push_back({ { 0.5f, 0.0f, -0.5f}, Colors::Blue });
	mesh.vertices.push_back({ { -0.5f, 0.0f, 0.5f }, Colors::Blue });
	mesh.vertices.push_back({ { -0.5f, 1.0f, 0.5f }, Colors::Blue });
	mesh.vertices.push_back({ { 0.5f, 1.0f, 0.5f }, Colors::Blue });
	mesh.vertices.push_back({ { 0.5f, 0.0f, 0.5f }, Colors::Blue });

	mesh.indices =
	{
		//front
		0, 1, 2,
		0, 2, 3,
		//right
		3, 2, 6,
		3, 6, 7,
		//back
		7, 6, 5,
		7, 5, 4,
		//left
		4, 5, 1,
		4, 1, 0,
		//top
		1, 5, 6,
		1, 6, 2,
		//bottom
		3, 7, 4,
		3, 4, 0
	};
	mMeshBuffer.Initialize<MeshPC>(mesh);
}
void GameState::Terminate()
{
	mMeshBuffer.Terminate();
	mTransformBuffer.Terminate();
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

	Matrix4 matWorld = Matrix4::Identity;
	Matrix4 matView = mCamera.GetViewMatrix();
	Matrix4 matProj = mCamera.GetProjectionMatrix();
	Matrix4 matFinal = matWorld * matView * matProj;
	Matrix4 wvp = Transpose(matFinal);

	mTransformBuffer.Update(&wvp);
	mTransformBuffer.BindVS(0);


	mMeshBuffer.Render();
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