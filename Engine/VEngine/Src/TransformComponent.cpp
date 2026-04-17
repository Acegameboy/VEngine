#include "Precompiled.h"
#include "TransformComponent.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void VEngine::TransformComponent::DebugUI()
{
	ImGui::DragFloat3("Position", &position.x, 0.1f);
	ImGui::DragFloat4("Rotation", &rotation.x, 0.0001f);
	ImGui::DragFloat3("Scale", &scale.x, 0.1f);

	SimpleDraw::AddTransform(GetMatrix4());
}
