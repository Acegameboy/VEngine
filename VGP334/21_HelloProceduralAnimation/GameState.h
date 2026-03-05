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
	VEngine::Graphics::StandardEffect mStandardEffect;

	// Ground info
	VEngine::Graphics::RenderObject mGroundObject;
	VEngine::Physics::CollisionShape mGroundShape;
	VEngine::Physics::RigidBody mGroundRigidBody;

	struct Segment
	{
		VEngine::Graphics::RenderObject segment;
		float range = 0.0f;
	};

	std::vector<Segment> mSegments;
	VEngine::Physics::CollisionShape mBodyAnchorShape;
	VEngine::Physics::RigidBody mBodyAnchor;
};