#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
	mPosition = Math::Vector3::Zero;
	mLineA = Math::Vector3::Zero;
	mLineB = Math::Vector3::Zero;

	std::filesystem::path shaderFile = L"../../Assets/Shaders/DoTransformTexture.fx";
	mVertexShader.Initialize<VertexPX>(shaderFile);
	mPixelShader.Initialize(shaderFile);
	mTransformBuffer.Initialize(sizeof(Matrix4));

	MeshPX mesh = MeshBuilder::CreateSpherePX(10, 10, 1.0f);

	mMeshBuffer.Initialize<MeshPX>(mesh);

	mTexture.Initialize(L"../../Assets/Textures/Images/Images/cat.bmp");
	mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}
void GameState::Terminate()
{
	mMeshBuffer.Terminate();
	mTransformBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mTexture.Terminate();
	mSampler.Terminate();
}
void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}
void GameState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	Matrix4 matWorld = Math::Matrix4::Translation(mPosition);
	Matrix4 matView = mCamera.GetViewMatrix();
	Matrix4 matProj = mCamera.GetProjectionMatrix();
	Matrix4 matFinal = matWorld * matView * matProj;
	Matrix4 wvp = Transpose(matFinal);

	mTransformBuffer.Update(&wvp);
	mTransformBuffer.BindVS(0);

	mTexture.BindPS(0);
	mSampler.BindPS(0);
	mMeshBuffer.Render();

	SimpleDraw::AddTransform(matWorld);
}
void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::DragFloat3("Position", &mPosition.x, 0.1f);
	ImGui::DragFloat3("LineA", &mLineA.x, 0.1f);
	ImGui::DragFloat3("LineB", &mLineB.x, 0.1f);
	ImGui::ColorEdit4("LineColor", &mLineColor.r);

	ImGui::Checkbox("GroundPlane", &mShowGroundPlane);
	ImGui::Checkbox("ShowAABB", &mShowAABB);
	ImGui::Checkbox("ShowFilledAABB", &mShowFilledAABB);
	ImGui::End();

	if (mShowGroundPlane)
	{
		SimpleDraw::AddGroundPlane(20, Colors::White);
	}
	if (mShowAABB)
	{
		SimpleDraw::AddAABB({ -1.0f, 0.0f, -1.0f }, { 1.0f, 2.0f, 1.0f }, Colors::Green);
	}
	if (mShowFilledAABB)
	{
		SimpleDraw::AddFilledAABB({ -1.0f, 0.0f, -1.0f }, { 1.0f, 2.0f, 1.0f }, Colors::Green);
	}

	SimpleDraw::AddLine(mLineA, mLineB, mLineColor);
	SimpleDraw::Render(mCamera);
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