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

const char* planetNames[] =
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
	mCamera.SetPosition({ 0.0f, 1.0f, -25.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
	mRenderTargetCamera.SetAspectRatio(1.0f);

	mPointLight.position = { 0.0f, 0.0f, 0.0f };  
	mPointLight.range = 200.0f;

	std::filesystem::path shaderFile = L"../../Assets/Shaders/PointLight.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
	mStandardEffect.SetPointLight(mPointLight); //Sun Light

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
	std::vector<float> dfs = { 0.0f, 35.9f, 67.2f, 92.9f, 141.6f, 483.8f, 890.7f, 1787.0f, 2798.0f, 3700.0f };
	std::vector<float> orbitSpeed = { 0.0f, 4.15f, 1.61f, 1.0f, 0.53f, 0.084f, 0.034f, 0.011f, 0.006f, 0.004f };
	std::vector<float> rotSpeed = { 0.0f, 0.002f, -0.0005f, 1.0f, 0.98f, 2.4f, 2.2f, -1.4f, 1.5f, -0.2f };
	
	//Create planet data according to string
	planets.resize(planetTextures.size());
	for (size_t i = 0; i < planetTextures.size(); i++)
	{
		Planet& planet = planets[i];
		planet.name = planetTextures[i];
		planet.dfs = dfs[i];
		planet.orbitSpeed = orbitSpeed[i];

		planet.rotationSpeed = rotSpeed[i];
		planet.angle = 0.0f;
		planet.selfRotation = 0.0f;
		planet.planetSize = planetSize[i];

		Mesh mesh = MeshBuilder::CreateSphere(20.0f, 20.f, planetSize[i]);
		planet.renderObject.meshBuffer.Initialize<Mesh>(mesh);
		planet.renderObject.texture.Initialize(L"../../Assets/Textures/planets/" + std::wstring(planetTextures[i].begin(), planetTextures[i].end()));
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
		planet.selfRotation += Math::Constants::Pi * (planet.rotationSpeed * 0.1f) * deltaTime;
		if (planet.dfs > 0.0f)
		{
			planet.angle += Math::Constants::Pi * (planet.orbitSpeed * 0.1f) * deltaTime;
		}
	}
}
void GameState::Render()
{
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.Begin();

	//Render Skybox
	skybox.transform.position = { 0.0f, 0.0f, 0.0f };
	mStandardEffect.Render(skybox);

	//Render Orbit Rings
	if (showOrbitRings)
	{
		for (auto& planet : planets)
		{
			if (planet.dfs > 0.0f) 
			{
				SimpleDraw::AddGroundCircle(100, planet.dfs, Colors::White);
			}

			planet.renderObject.transform.position = {
		   planet.dfs * cos(planet.angle),
		   0.0f,
		   planet.dfs * sin(planet.angle)};

			planet.renderObject.transform.rotation = Math::Quaternion::CreateFromAxisAngle(
				Math::Vector3::YAxis, planet.selfRotation);
		}
	}

	//Render Grid
	if (mShowGrid)
	{
		SimpleDraw::AddGroundPlane(500.0f, Colors::White);
	}

	//Render Planets
	for (size_t i = 0; i < planets.size(); ++i)
	{
		Planet& planet = planets[i];

		planet.renderObject.transform.position = {
			planet.dfs * cos(planet.angle),
			0.0f,
			planet.dfs * sin(planet.angle)
		};

		if (mWireframeMode)
		{
			Color wireframeColor = Colors::White;
			switch (i)
			{
			case 0: wireframeColor = Colors::Yellow; break; // Sun
			case 1: wireframeColor = Colors::Gray; break; // Mercury
			case 2: wireframeColor = Colors::Orange; break; // Venus
			case 3: wireframeColor = Colors::Blue; break; // Earth
			case 4: wireframeColor = Colors::Red; break; // Mars
			case 5: wireframeColor = Colors::Brown; break; // Jupiter
			case 6: wireframeColor = Colors::LightGray; break; // Saturn
			case 7: wireframeColor = Colors::Cyan; break; // Uranus
			case 8: wireframeColor = Colors::DarkBlue; break; // Neptune
			}

			SimpleDraw::AddSphere(16, 16, planet.planetSize, planet.renderObject.transform.position, wireframeColor);
		}
	}
	mStandardEffect.End();

	//Render Target Camera
	if (mCurrentSelection >= 0 && mCurrentSelection < planets.size())
	{
		mRenderTarget.BeginRender();
		mStandardEffect.SetCamera(mRenderTargetCamera);
		mStandardEffect.Begin();

		Planet& focusedPlanet = planets[mCurrentSelection];
		
		float planetSize = focusedPlanet.planetSize;

		Vector3 planetPos = focusedPlanet.renderObject.transform.position;
		Vector3 cameraOffset = { 0.0f, 0.0f, -planetSize * 5.0f };

		mRenderTargetCamera.SetPosition(planetPos + cameraOffset);
		mRenderTargetCamera.SetLookAt(planetPos);

		focusedPlanet.renderObject.texture.BindPS(0);
		mStandardEffect.Render(focusedPlanet.renderObject);
		mStandardEffect.End();
		mRenderTarget.EndRender();
	}

	SimpleDraw::Render(mCamera);
}
void GameState::DebugUI()
{
	ImGui::Begin("Solar System", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

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
		ImGui::ColorEdit4("Specular", &mPointLight.specular.r);
		if (ImGui::DragFloat("Direction", &mDirectionalLight.direction.x, 0.01f))
		{
			mDirectionalLight.direction = Normalize(mDirectionalLight.direction);
		}
	}
	ImGui::PopID();

	ImGui::PushID("Toggles");
	if (ImGui::CollapsingHeader("Toggles", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Show Orbit Rings", &showOrbitRings);
		ImGui::Checkbox("Show Grid", &mShowGrid);
	}
	ImGui::PopID();

	ImGui::Combo("Select Planet", &mCurrentSelection, planetNames, IM_ARRAYSIZE(planetNames));

	ImGui::Checkbox("Wireframe Mode", &mWireframeMode);

	if (mCurrentSelection >= 0 && mCurrentSelection < planets.size())
	{
		Planet& selectedPlanet = planets[mCurrentSelection];

		float& newRotationSpeed = planets[mCurrentSelection].rotationSpeed;
		float& newOrbitSpeed = planets[mCurrentSelection].orbitSpeed;

		if (ImGui::DragFloat("Rotation Speed", &newRotationSpeed, 0.01f))
		{
			selectedPlanet.rotationSpeed = Math::Clamp(newRotationSpeed, -5.0f, 5.0f);
		}
		if (ImGui::DragFloat("Orbit Speed", &newOrbitSpeed, 0.01f))
		{
			selectedPlanet.orbitSpeed = Math::Clamp(newOrbitSpeed, 0.0f, 10.0f);
		}

		ImGui::Image(
			mRenderTarget.GetRawData(),
			{ 128, 128 },
			{ 0, 0 },
			{ 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 });
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