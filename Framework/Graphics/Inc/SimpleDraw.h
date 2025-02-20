#pragma once

#include "Colors.h"

namespace VEngine::Graphics
{
	class Camera;

	namespace SimpleDraw
	{
		void StaticInitialize(uint32_t maxVertexCount);
		void StaticTerminate();

		void AddLine(const Math::Vector3& v0, const Math::Vector3& v1, const Color& color);
		void AddFace(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, const Color& color);

		void AddAABB(const Math::Vector3& min, const Math::Vector3& max, const Color& color);
		void AddAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, const Color& color);

		void AddFilledAABB(const Math::Vector3& min, const Math::Vector3& max, const Color& color);
		void AddFilledAABB(float minX, float minY, float minZ, float maxX, float maxY, float maxZ, const Color& color);

		void AddSphere(uint32_t slices, uint32_t rings, float radius, const Color& color);
		void AddSphere(uint32_t slices, uint32_t rings, float radius, const Math::Vector3& offset, const Color& color);

		void AddGroundPlane(float size, const Color& color);
		void AddGroundCircle(uint32_t slices, float radius, const Color& color);

		void AddTransform(const Math::Matrix4& matrix);

		void Render(const Camera& camera);
	}
}