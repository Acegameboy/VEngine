#include "Precompiled.h"
#include "UIButtonComponent.h"
#include "GameWorld.h"
#include"UIRenderService.h"
#include "UISpriteComponent.h"

using namespace VEngine;
using namespace VEngine::Graphics;
using namespace VEngine::Input;

void VEngine::UIButtonComponent::Initialize()
{
	for (ButtonStateEntry& buttonState : mButtonStates)
	{
		if (!buttonState.texture.empty())
		{
			buttonState.sprite.Initialize(buttonState.texture);
		}
	}

	UIRenderService* uiRenderService = GetOwner().GetWorld.GetService<UIRenderService>();
	uiRenderService->Register(this);
}

void VEngine::UIButtonComponent::Terminate()
{
	UIRenderService* uiRenderService = GetOwner().GetWorld.GetService<UIRenderService>();
	uiRenderService->Unregister(this);
}

void VEngine::UIButtonComponent::Update(float deltaTime)
{
	if (mCurrentState == ButtonState::Default)
	{
		return;
	}
	mCurrentState = ButtonState::Default;
	InputSystem* input = InputSystem::Get();
	const int mouseX = input->GetMouseScreenX();
	const int mouseY = input->GetMouseScreenY();
	if (mButtonState[0].sprite.IsInSprite(mouseX, mouseY))
	{
		mCurrentState = ButtonState::Click;
	}
	if()
}

void VEngine::UIButtonComponent::Render()
{
	uint32_t buttonStateIndex = static_cast<uint32_t>(mCurrentState);
	if (mButtonState[buttonStateIndex].texture.empty())
	{
		buttonStateIndex = 0;
	}
	Math::Vector2 worldPosition = GetPosition(false);
	GameObject* parent = GetOwner().GetParent();
	while (parent != nullptr)
	{
		UISpriteComponent* spriteComponent = parent->GetComponent<UIButtonComponent>();
		if (spriteComponent != nullptr)
		{
			worldPosition += spriteComponent->GetPosition();
		}
	}
}

void VEngine::UIButtonComponent::Deserialize(const rapidjson::Value& value)
{
	const uint32_t buttonStateCount = static_cast<uint32_t>(ButtonState::Count);
	if (value.HasMember("Position"))
	{
		auto pos = value["Position"].GetArray();
		mPosition.x = pos[0].GetFloat();
		mPosition.y = pos[0].GetFloat();
	}
	if (value.HasMember("Rotation"))
	{
		const float rotation = value["Rotation"].GetFloat();
		for (ButtonStateEntry& buttonState : mButtonStates)
		{
			buttonState.sprite.SetRotation(rotation);
		}
		if (value.HasMember("Pivot"))
		{
			Pivot buttonPivot = Pivot::TopLeft;

		}
	}
}

Math::Vector2 VEngine::UIButtonComponent::GetPosition(bool includeOrigin)
{
	return Math::Vector2();
}

void VEngine::UIButtonComponent::SetCallback(ButtonCallback cb)
{
}

void VEngine::UIButtonComponent::OnClick()
{
}
