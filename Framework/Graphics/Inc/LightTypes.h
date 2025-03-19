#pragma once

#include "Colors.h"

namespace VEngine::Graphics
{
	struct DirectionalLight
	{
		Color ambient = Colors::White;
		Color diffuse = Colors::White;
		Color specular = Colors::White;
		Math::Vector3 direction = -Math::Vector3::YAxis;
		float padding = 0.0f;
	};

	struct PointLight
	{
		VEngine::Math::Vector3 position;
		float range;
		VEngine::Math::Vector4 ambient;
		VEngine::Math::Vector4 diffuse;
		VEngine::Math::Vector4 specular;
	};
}
