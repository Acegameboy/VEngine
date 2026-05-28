#pragma once

#include "UIComponent.h"

namespace VEngine
{
	enum class ButtonState
	{
		Default,
		Hover,
		Click,
		Disabled,
		Count
	};

	using ButtonCallback = std::function<void>();

	class UIButtonComponent : public UIComponent
	{
	public:
		SET_TYPE_ID(ComponentId::UIButton);

		void Initialize() override;
		void Terminate() override;
		void Update(float deltaTime) override;
		void Render() override;
		void Deserialize(const rapidjson::Value& value) override;

		Math::Vector2 GetPosition(bool includeOrigin = true);
		void SetCallback(ButtonCallback cb);

	private:
		void OnClick();

		struct ButtonStateEntry
		{
			std::string texture;
			Graphics::UISprite sprite;
		};
		using ButtonStates = std::array<ButtonStateEntry, static_cast<uint32_t>(ButtonState)>

	};
}