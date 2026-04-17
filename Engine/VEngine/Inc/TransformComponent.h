#pragma once

#include "Component.h"

namespace VEngine
{
	class TransformComponent final : public Component, public Graphics::Transform
	{
	public:
		SET_TYPE_ID(ComponentId::Transfrom);

		void DebugUI() override;
		

	};
}
