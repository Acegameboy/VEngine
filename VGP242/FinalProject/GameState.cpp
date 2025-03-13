#include "GameState.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;
using namespace VEngine::Math;

struct Planet {
	std::string name;
	float planetSize;
	float dfs; //distance from sun
	float orbitSpeed;
	float rotationSpeed;
	float tilt;
	float angle; 
	float selfRotation; 
	RenderObject renderObject;
	bool showOrbit = true; 
};



std::vector<Planet> planets;
RenderObject skybox;

const char* plantetNames[] =
{
	"Sun",
	"Mercury",
	"Venus",
	"Earth",
	"Mars",
	"Jupiter",
	"Saturn",
	"Uranus",
	"Neptune"
};

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
	mRenderTargetCamera.SetAspectRatio(1.0f);

	mDirectionalLight.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };
	mDirectionalLight.diffuse = { 0.6f, 0.6f, 0.6f, 1.0f };
	mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };
	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });

	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);	

	//Skybox
	Mesh skyboxMesh = MeshBuilder::CreateSkySphere(100.0f, 100.f, 500.0f);
	skybox.meshBuffer.Initialize<Mesh>(skyboxMesh);
	skybox.texture.Initialize(L"../../Assets/Textures/space.jpg");

	constexpr uint32_t size = 512;
	mRenderTarget.Initialize(size, size, RenderTarget::Format::RGBA_U32);

	//Get planet textures
	std::vector<std::string> planetTextures = {
		"sun.jpg", "mercury.jpg", "venus.jpg", "earth.jpg", "mars.jpg",
		"jupiter.jpg", "saturn.jpg", "uranus.jpg", "neptune.jpg", "pluto.jpg"
	};

	//Planet data 
	std::vector<float> planetSize = { 20.0f, 0.4f, 0.95f, 1.0f, 0.53f, 11.2f, 9.45f, 4.0f, 3.88f, 0.18f };
	std::vector<float> dfs = { 0.0f, 5.0f, 10.0f, 15.0f, 22.5f, 40.0f, 55.0f, 70.0f, 85.0f, 100.0f };
	std::vector<float> orbitSpeed = { 0.0f, 4.15f, 1.61f, 1.0f, 0.53f, 0.084f, 0.034f, 0.011f, 0.006f, 0.004f };
	std::vector<float> rotSpeed = { 0.0f, 0.002f, -0.0005f, 1.0f, 0.98f, 2.4f, 2.2f, -1.4f, 1.5f, -0.2f };
	
	//Create planet data according to string
	for (size_t i = 0; i < planetTextures.size(); i++)
	{
		Planet planet;
		planet.name = planetTextures[i];
		planet.dfs = dfs[i];
		planet.orbitSpeed = orbitSpeed[i];

		planet.rotationSpeed = rotSpeed[i];
		planet.angle = 0.0f;
		planet.selfRotation = 0.0f;

		Mesh mesh = MeshBuilder::CreateSphere(20.0f, 20.f, planetSize[i]);
		planet.renderObject.meshBuffer.Initialize<Mesh>(mesh);
		mRenderObject.texture.Initialize(L"../../Assets/Textures/planets/" + std::wstring(planetTextures[i].begin(), planetTextures[i].end()));
		planets.emplace_back(std::move(planet));
	}
}
void GameState::Terminate()
{
	mRenderTarget.Terminate();
	skybox.Terminate();
	mStandardEffect.Terminate();
	for (auto& planet : planets)
	{
		planet.renderObject.meshBuffer.Terminate();
		planet.renderObject.texture.Terminate();
	}

}
void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	for (auto& planet : planets) 
	{
		planet.selfRotation += Math::Constants::Pi * planet.rotationSpeed * deltaTime;
		if (planet.dfs > 0.0f) 
		{
			planet.angle += Math::Constants::Pi * planet.orbitSpeed * deltaTime;
		}
	}
}
void GameState::Render()
{
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.Begin();

	skybox.transform.position = { 0.0f, 0.0f, 0.0f };
	mStandardEffect.Render(skybox);

	for (auto& planet : planets) 
	{
		planet.renderObject.transform.position = {
			planet.dfs * cos(planet.angle),
			0.0f,
			planet.dfs * sin(planet.angle)
		};

		planet.renderObject.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, planet.selfRotation);
		planet.renderObject.texture.BindPS(0);
		mStandardEffect.Render(planet.renderObject);

		if (planet.showOrbit) 
		{
			SimpleDraw::AddGroundCircle(60, planet.dfs, Colors::White);
		}
	}

	mStandardEffect.End();
}
void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushID("Material");
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Emissive", &mRenderObject.material.emissive.r);
		ImGui::ColorEdit4("Ambient", &mRenderObject.material.ambient.r);
		ImGui::ColorEdit4("Diffuse", &mRenderObject.material.diffuse.r);
		ImGui::ColorEdit4("Specular", &mRenderObject.material.specular.r);
		ImGui::DragFloat("Shineness", &mRenderObject.material.shininess, 0.1f, 0.0f, 100.0f);
	}
	ImGui::PopID();
	ImGui::PushID("Lighting");
	if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Ambient", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular", &mDirectionalLight.specular.r);
		if (ImGui::DragFloat("Direction", &mDirectionalLight.direction.x, 0.01f))
		{
			mDirectionalLight.direction = Normalize(mDirectionalLight.direction);
		}
	}
	ImGui::PopID();
	ImGui::Image(
		mRenderTarget.GetRawData(),
		{ 128, 128 },
		{ 0, 0 },
		{ 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 });
	if (ImGui::Combo("ComboBox", &mCurrentSelection, plantetNames, std::size(plantetNames)))
	{
		//do stuff if the current selection changes
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