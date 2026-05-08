#include "CustomDebugDrawComponent.h"
#include "CustomDebugDrawService.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Math;

void CustomDebugDrawComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
}

void CustomDebugDrawComponent::Terminate()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	CustomDebugDrawService* debugDrawService = GetOwner.GetWorld()
}

void CustomDebugDrawComponent::DebugUI()
{
	ImGui::DragFloat3("Position", &mPosition.x);
	ImGui::ColorEdit4("Color", &mColor.r);
}

void CustomDebugDrawComponent::Deserialize(const rapidjson::Value& value)
{
	int slices = mSlices;
	int rings = mRings;
	SaveUtil::ReadInt("Slices", slices, value);
	SaveUtil::ReadInt("Rings", rings, value);
	SaveUtil::ReadFloat("Radius", mRadius, value);
	SaveUtil::ReadVector3("Position", mPosition, value);
	SaveUtil::ReadColor("Color", mColor, value);
	mSlices = slices;
	mRings = rings;
}

void CustomDebugDrawComponent::AddDebugDraw() const
{
	Vector3 worldSpace = mPosition;
	if (mTransformComponent != nullptr)
	{
		worldSpace = TransformCoord(mPosition, mTransformComponent->GetMatrix4());
	}
	SimpleDraw::AddSphere(mSlices, mRings, mRadius, worldSpace, mColor);
}
