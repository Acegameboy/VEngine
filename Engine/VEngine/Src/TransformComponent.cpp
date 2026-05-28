#include "Precompiled.h"
#include "TransformComponent.h"
#include "SaveUtil.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void VEngine::TransformComponent::DebugUI()
{
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	ImGui::DragFloat4("Rotation", &rotation.x, 0.0001f);
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);

	SimpleDraw::AddTransform(GetMatrix4());
}

void VEngine::TransformComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadVector3("Position", position, value);
	SaveUtil::ReadQuaternion("Rotation", rotation, value);
	SaveUtil::ReadVector3("Scale", scale, value);
}

Transform VEngine::TransformComponent::GetWorldTransform() const
{
	Transform worldTransform = *this;
	const GameObject* parent = GetOwner().GetParent();
	if (parent != nullptr)
	{
		Math::Matrix4 matWorld = GetMatrix4();
		while (parent != nullptr)
		{
			const TransformComponent* transformComponent = parent->GetComponent<TransformComponent>();
			ASSERT(transformComponent != nullptr, "TransformComponent: parent doesnt have a transform");
			matWorld = matWorld * transformComponent->GetMatrix4();
			parent = parent->GetParent();
		}
		worldTransform.position = Math::GetTranslation(matWorld);
		worldTransform.rotation = Math::Quaternion::CreateFromRotationMatrix(matWorld);
		worldTransform.scale = Math::GetScale(matWorld);
	}
	return worldTransform;
}
