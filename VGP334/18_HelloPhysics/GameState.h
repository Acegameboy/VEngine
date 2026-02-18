#pragma once

#include <VEngine/Inc/VEngine.h>

class GameState : public VEngine::AppState
{
public:
	void Initialize() override;

	void Terminate() override;

	void Update(float deltaTime) override;

	void Render() override;

	void DebugUI() override;

private:

	void UpdateCamera(float deltaTime);

	VEngine::Graphics::Camera mCamera;
	VEngine::Graphics::DirectionalLight mDirectionalLight;

	// Ball info
	VEngine::Graphics::RenderObject mFootball;
	VEngine::Physics::CollisionShape mBallShape;
	VEngine::Physics::RigidBody mBallRigidBody;

	// Ground info
	VEngine::Graphics::RenderObject mGroundObject;
	VEngine::Physics::CollisionShape mGroundShape;
	VEngine::Physics::RigidBody mGroundRigidBody;

	struct BoxData
	{
		VEngine::Graphics::RenderObject box;
		VEngine::Physics::CollisionShape shape;
		VEngine::Physics::RigidBody rigidBody;
	};
	using Boxes = std::vector<BoxData>;
	Boxes mBoxes;

	VEngine::Graphics::StandardEffect mStandardEffect;

	// Cloth info
	VEngine::Graphics::RenderObject mCloth;
	VEngine::Graphics::Mesh mClothMesh;
	VEngine::Physics::SoftBody mClothSoftBody;
};