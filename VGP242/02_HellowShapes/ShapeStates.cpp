#include "ShapeStates.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Math;
using namespace VEngine::Input;

void ShapeState::Initialize()
{
	std::filesystem::path shaderPath = L"../../Assets/Shaders/DoSomething.fx";
	mVertexShader.Initialize<VertexP>(shaderPath);
	mPixelShader.Initialize(shaderPath);

	std::vector<VertexP> vertices;
	vertices.push_back({ Vector3(-0.75f, -0.75f, 0.0f) });
	vertices.push_back({ Vector3(0.0f, 0.75f, 0.0f) });
	vertices.push_back({ Vector3(0.75f, -0.75f, 0.0f) });
	mMeshBuffer.Initialize<VertexP>(vertices);
}

void ShapeState::Terminate()
{
	mMeshBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
}

void ShapeState::Update(float deltaTime)
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::ENTER))
	{
		MainApp().ChangeState("ColoredShapeState");
	}
}

void ShapeState::Render()
{
	mVertexShader.Bind();
	mPixelShader.Bind();

	mMeshBuffer.Render();
}

void ColoredShapeState::Initialize()
{
	std::filesystem::path shaderPath = L"../../Assets/Shaders/DoColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderPath);
	mPixelShader.Initialize(shaderPath);

	std::vector<VertexPC> vertices;
	vertices.push_back({ Vector3(-0.75f, -0.75f, 0.0f), Colors::Red });
	vertices.push_back({ Vector3(0.0f, 0.75f, 0.0f), Colors::Blue });
	vertices.push_back({ Vector3(0.75f, -0.75f, 0.0f), Colors::Green });
	mMeshBuffer.Initialize<VertexPC>(vertices);
}

void ColoredShapeState::Update(float deltaTime)
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::ENTER))
	{
		MainApp().ChangeState("TriforceShapeState");
	}
}

void TriforceShapeState::Initialize()
{
	std::filesystem::path shaderPath = L"../../Assets/Shaders/DoColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderPath);
	mPixelShader.Initialize(shaderPath);

	std::vector<VertexPC> vertices;
	vertices.push_back({ Vector3(-0.75f, -0.75f, 0.0f), Colors::Red });
	vertices.push_back({ Vector3(-0.5f, 0.0f, 0.0f), Colors::Blue });
	vertices.push_back({ Vector3(0.0f, -0.75f, 0.0f), Colors::Green });

	vertices.push_back({ Vector3(-0.5f, 0.0f, 0.0f), Colors::Red });
	vertices.push_back({ Vector3(0.0f, 0.5f, 0.0f), Colors::Blue });
	vertices.push_back({ Vector3(0.5f, 0.0f, 0.0f), Colors::Green });

	vertices.push_back({ Vector3(0.75f, -0.75f, 0.0f), Colors::Red });
	vertices.push_back({ Vector3(0.5f, 0.0f, 0.0f), Colors::Blue });
	vertices.push_back({ Vector3(0.0f, -0.75f, 0.0f), Colors::Green });
	mMeshBuffer.Initialize<VertexPC>(vertices);
}

void TriforceShapeState::Update(float deltaTime)
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::ENTER))
	{
		MainApp().ChangeState("DiamondShapeState");
	}
}

void DiamondShapeState::Initialize()
{
	std::filesystem::path shaderPath = L"../../Assets/Shaders/DoColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderPath);
	mPixelShader.Initialize(shaderPath);

	std::vector<VertexPC> vertices;
	vertices.push_back({ Vector3(0.75f, -0.75f, 0.0f), Colors::Red });   
	vertices.push_back({ Vector3(0.0f, -0.75f, 0.0f), Colors::Green });   
	vertices.push_back({ Vector3(0.5f, 0.0f, 0.0f), Colors::Blue });      

	vertices.push_back({ Vector3(0.5f, 0.0f, 0.0f), Colors::Blue });      
	vertices.push_back({ Vector3(0.0f, -0.75f, 0.0f), Colors::Green });  
	vertices.push_back({ Vector3(0.25f, 0.75f, 0.0f), Colors::Yellow });  

	vertices.push_back({ Vector3(0.5f, 0.0f, 0.0f), Colors::Blue });      
	vertices.push_back({ Vector3(0.75f, -0.75f, 0.0f), Colors::Red });   
	vertices.push_back({ Vector3(1.0f, 0.75f, 0.0f), Colors::Cyan });    
	mMeshBuffer.Initialize<VertexPC>(vertices);
}

void DiamondShapeState::Update(float deltaTime)
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::ENTER))
	{
		MainApp().ChangeState("HeartShapeState");
	}
}

void HeartShapeState::Initialize()
{
	std::filesystem::path shaderPath = L"../../Assets/Shaders/DoColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderPath);
	mPixelShader.Initialize(shaderPath);

	std::vector<VertexPC> vertices;
	vertices.push_back({ Vector3(0.5f, -0.75f, 0.0f), Colors::Red });    
	vertices.push_back({ Vector3(0.0f, -0.25f, 0.0f), Colors::Pink });   
	vertices.push_back({ Vector3(1.0f, -0.25f, 0.0f), Colors::Pink }); 

	vertices.push_back({ Vector3(0.0f, -0.25f, 0.0f), Colors::Pink });   
	vertices.push_back({ Vector3(0.0f, 0.5f, 0.0f), Colors::Magenta });  
	vertices.push_back({ Vector3(0.5f, 0.75f, 0.0f), Colors::Magenta }); 

	vertices.push_back({ Vector3(1.0f, -0.25f, 0.0f), Colors::Pink });   
	vertices.push_back({ Vector3(1.0f, 0.5f, 0.0f), Colors::Magenta });  
	vertices.push_back({ Vector3(0.5f, 0.75f, 0.0f), Colors::Magenta }); 

	vertices.push_back({ Vector3(0.0f, 0.5f, 0.0f), Colors::Magenta });  
	vertices.push_back({ Vector3(0.25f, 1.0f, 0.0f), Colors::Red });    
	vertices.push_back({ Vector3(0.5f, 0.75f, 0.0f), Colors::Magenta }); 

	vertices.push_back({ Vector3(1.0f, 0.5f, 0.0f), Colors::Magenta }); 
	vertices.push_back({ Vector3(0.75f, 1.0f, 0.0f), Colors::Red });     
	vertices.push_back({ Vector3(0.5f, 0.75f, 0.0f), Colors::Magenta }); 
	mMeshBuffer.Initialize<VertexPC>(vertices);
}

void HeartShapeState::Update(float deltaTime)
{
	if (InputSystem::Get()->IsKeyPressed(KeyCode::ENTER))
	{
		MainApp().ChangeState("ShapeState");
	}
}
