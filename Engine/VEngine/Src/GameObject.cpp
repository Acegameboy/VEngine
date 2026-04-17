#include "Precompiled.h"
#include "GameObject.h"

using namespace VEngine;

static uint32_t gUniqueId = 0;

void VEngine::GameObject::Initialize()
{
	ASSERT(!mInitialized, "GameObject: is already initizlized");
	for (auto& component : mComponents)
	{
		component->Initialize();
	}
	mId = ++gUniqueId;
	mInitialized = true;
}

void VEngine::GameObject::Terminate()
{
	for (auto& component : mComponents)
	{
		component->Terminate();
		component.reset();
	}
}

void VEngine::GameObject::Update(float deltaTime)
{
	component->Update(deltaTime);
}

void VEngine::GameObject::DebugUI()
{
	ImGui::PushID(mId);
	if (ImGui::CollapsingHeader(mName.c_str()))
	{
		//add stuff later
	}
	ImGui::PopID();
	for (auto& component : mComponents)
	{
		component->DebugUI();
	}
}

void VEngine::GameObject::SetName(std::string& name)
{
	mName = std::move(name);
}

const std::string& VEngine::GameObject::GetName() const
{
	return mName;
}

uint32_t VEngine::GameObject::GetId() const
{
	return mId;
}

const GameObjectHandle& VEngine::GameObject::GetHandle() const
{
	return mHandle;
}
