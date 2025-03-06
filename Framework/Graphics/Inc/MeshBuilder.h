#pragma once

#include "MeshTypes.h"

namespace VEngine::Graphics
{
	class MeshBuilder
	{
	public:
		//pyramid
		static MeshPC CreatePyramidPC(float size);

		//cube
		static MeshPC CreateCubePC(float size);
		//rectangle
		static MeshPC CreateRectPC(float width, float height, float depth);
		//Plane
		static MeshPC CreateVerticalPlanePC(uint32_t rows, uint32_t cols, float spacing);
		static MeshPC CreateHorizontalPlanePC(uint32_t rows, uint32_t cols, float spacing);
		//Cylinder
		static MeshPC CreateCylinderPC(uint32_t slices, uint32_t rings);
		//Sphere
		static MeshPC CreateSpherePC(uint32_t slices, uint32_t rings, float radius);
		static MeshPX CreateSpherePX(uint32_t slices, uint32_t rings, float radius);
		static MeshPX CreateSkySpherePX(uint32_t slices, uint32_t rings, float radius);

		static Mesh CreateSphere(uint32_t slices, uint32_t rings, float radius);
		static Mesh CreateSkySphere(uint32_t slices, uint32_t rings, float radius);
	};
}
