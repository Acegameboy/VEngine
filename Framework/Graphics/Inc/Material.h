#pragma once

#include "Colors.h"

namespace VEngine::Graphics
{
	struct Material
	{
		Color emissive = Colors::Black;
		Color ambient = Colors::White;
		Color diffuse = Colors::White;
		Color specular = Colors::White;
		float shininess = 10.0f;
		float padding[3] = { 0.0f };
	};
}
