#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mDirectionalLight.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	mDirectionalLight.diffuse = { 0.6f, 0.6f, 0.6f, 1.0f };
	mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };
	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });

	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	
	Mesh mesh = MeshBuilder::CreateSphere(10, 10, 1.0f);
	mRenderObject.meshBuffer.Initialize<Mesh>(mesh);
	mRenderObject.texture.Initialize(L"../../Assets/Textures/Images/Images/cat.bmp");
}
void GameState::Terminate()
{
	mRenderObject.Terminate();
	mStandardEffect.Terminate();
}
void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}
void GameState::Render()
{
	mStandardEffect.Begin();
		mStandardEffect.Render(mRenderObject);
	mStandardEffect.End();
}
void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Emissive", &mRenderObject.material.emissive.r);
		ImGui::ColorEdit4("Ambient", &mRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse", &mRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular", &mRenderObject.material.specular.r);
		ImGui::DragFloat("Shineness", &mRenderObject.material.shininess, 0.1f, 0.0f, 100.0f);
	}
	mStandardEffect.DebugUI();
	ImGui::End();
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