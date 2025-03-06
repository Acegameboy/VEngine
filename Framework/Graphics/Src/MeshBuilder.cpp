#include "Precompiled.h"
#include "MeshBuilder.h"

using namespace VEngine;
using namespace VEngine::Graphics;

namespace
{
	Color GetNextColor(int& index)
	{
		constexpr Color colorTable[] =
		{
			Colors::Red,
			Colors::Green,
			Colors::Blue,
			Colors::DarkCyan,
			Colors::Aqua,
			Colors::Yellow,
			Colors::Pink
		};
		index = (index + 1) % std::size(colorTable);
		return colorTable[index];
	}

	void CreateCubeIndices(std::vector<uint32_t>& indices)
	{
		indices = {
			//front
			0, 1, 2,
			0, 2, 3,
			//back
			7, 5, 4,
			7, 6, 5,
			//right
			3, 2, 6,
			3, 6, 7,
			//left
			4, 5, 1,
			4, 1, 0,
			//top
			1, 5, 6,
			1, 6, 2,
			//bottom
			0, 3, 7,
			0, 7, 4,
		};
	}
	void CreatePlaneIndices(std::vector<uint32_t>& indices, uint32_t rows, uint32_t cols)
	{
		for (uint32_t r = 0; r < rows; ++r)
		{
			for (uint32_t c = 0; c < cols; ++c)
			{
				uint32_t i = c + (r * (cols + 1));
				//triangle 1
				indices.push_back(i);
				indices.push_back(i + cols + 2);
				indices.push_back(i + 1);
				//triangle 2
				indices.push_back(i);
				indices.push_back(i + cols + 1);
				indices.push_back(i + cols + 2);
			}
		}
	}
}

MeshPC MeshBuilder::CreatePyramidPC(float size)
{
	MeshPC mesh;
	const float hs = size * 0.5f;

	srand(time(nullptr));
	int index = rand() % 100;
	//front
	mesh.vertices.push_back({ {-hs, -hs, -hs}, GetNextColor(index) });
	mesh.vertices.push_back({ {0.0f, hs, 0.0f}, GetNextColor(index) });
	mesh.vertices.push_back({ {hs, -hs, -hs}, GetNextColor(index) });

	//back
	mesh.vertices.push_back({ {-hs, -hs, hs}, GetNextColor(index) });
	mesh.vertices.push_back({ {hs, -hs, hs}, GetNextColor(index) });

	//indices
	mesh.indices = {
		//front
		0, 1, 2,
		//right
		2, 1, 4,
		//back
		4, 1, 3,
		//left
		0, 3, 1,
		//bottom
		0, 2, 4,
		0, 4, 3,
	};
	return mesh;
}

MeshPC MeshBuilder::CreateCubePC(float size)
{
	MeshPC mesh;
	const float hs = size * 0.5f;

	srand(time(nullptr));
	int index = rand() % 100;
	//front
	mesh.vertices.push_back({ {-hs, -hs, -hs}, GetNextColor(index) });
	mesh.vertices.push_back({ {-hs,  hs, -hs}, GetNextColor(index) });
	mesh.vertices.push_back({ { hs,  hs, -hs}, GetNextColor(index) });
	mesh.vertices.push_back({ { hs, -hs, -hs}, GetNextColor(index) });

	//back
	mesh.vertices.push_back({ {-hs, -hs, hs}, GetNextColor(index) });
	mesh.vertices.push_back({ {-hs,  hs, hs}, GetNextColor(index) });
	mesh.vertices.push_back({ { hs,  hs, hs}, GetNextColor(index) });
	mesh.vertices.push_back({ { hs, -hs, hs}, GetNextColor(index) });

	mesh.indices = {
		//front
		0, 1, 2,
		0, 2, 3,
		//back
		7, 5, 4,
		7, 6, 5,
		//right
		3, 2, 6,
		3, 6, 7,
		//left
		4, 5, 1,
		4, 1, 0,
		//top
		1, 5, 6,
		1, 6, 2,
		//bottom
		0, 3, 7,
		0, 7, 4,
	};
	return mesh;
}

MeshPC VEngine::Graphics::MeshBuilder::CreateRectPC(float width, float height, float depth)
{
	MeshPC mesh;
	const float hw = width * 0.5f;
	const float hh = height * 0.5f;
	const float hd = depth * 0.5f;

	srand(time(nullptr));
	int index = rand() % 100;

	//front
	mesh.vertices.push_back({ {-hw, -hh, -hd}, GetNextColor(index) });
	mesh.vertices.push_back({ {-hw,  hh, -hd}, GetNextColor(index) });
	mesh.vertices.push_back({ { hw,  hh, -hd}, GetNextColor(index) });
	mesh.vertices.push_back({ { hw, -hh, -hd}, GetNextColor(index) });

	//back
	mesh.vertices.push_back({ {-hw, -hh, hd}, GetNextColor(index) });
	mesh.vertices.push_back({ {-hw,  hh, hd}, GetNextColor(index) });
	mesh.vertices.push_back({ { hw,  hh, hd}, GetNextColor(index) });
	mesh.vertices.push_back({ { hw, -hh, hd}, GetNextColor(index) });

	CreateCubeIndices(mesh.indices);

	return mesh;
}

MeshPC MeshBuilder::CreateVerticalPlanePC(uint32_t rows, uint32_t cols, float spacing)
{
	srand(time(nullptr));
	int index = rand() % 100;

	MeshPC mesh;

	const float hpw = static_cast<float>(cols) * spacing * 0.5f;
	const float hph = static_cast<float>(cols) * spacing * 0.5f;

	float x = -hpw;
	float y = -hph;

	for (uint32_t r = 0; r <= rows; ++r)
	{
		for (uint32_t c = 0; c <= cols; ++c)
		{
			mesh.vertices.push_back({ {x, y , 0.0f}, GetNextColor(index) });
			x += spacing;
		}
		x = -hpw;
		y += spacing;
	}

	CreatePlaneIndices(mesh.indices, rows, cols);
	return mesh;
}

MeshPC MeshBuilder::CreateHorizontalPlanePC(uint32_t rows, uint32_t cols, float spacing)
{
	srand(time(nullptr));
	int index = rand() % 100;

	MeshPC mesh;

	const float hpw = static_cast<float>(cols) * spacing * 0.5f;
	const float hph = static_cast<float>(cols) * spacing * 0.5f;

	float x = -hpw;
	float z = -hph;

	for (uint32_t r = 0; r <= rows; ++r)
	{
		for (uint32_t c = 0; c <= cols; ++c)
		{
			mesh.vertices.push_back({ {x, 0.0f , z}, GetNextColor(index) });
			x += spacing;
		}
		x = -hpw;
		z += spacing;
	}

	CreatePlaneIndices(mesh.indices, rows, cols);
	return mesh;
}

MeshPC MeshBuilder::CreateCylinderPC(uint32_t slices, uint32_t rings)
{
	srand(time(nullptr));
	int index = rand() % 100;

	MeshPC mesh;
	
	const float hh = static_cast<float>(rings) * 0.5f;

	for (uint32_t r = 0; r <= rings; ++r)
	{
		float ring = static_cast<float>(r);
		for (uint32_t s = 0; s <= slices; ++s)
		{
			float slice = static_cast<float>(s);
			float rotation = (slice / (static_cast<float>(slices))) * Math::Constants::TwoPi;
			mesh.vertices.push_back({ {
				cos(rotation),
				ring - hh,
				sin(rotation)},
				GetNextColor(index) });
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	mesh.vertices.push_back({ {0.0f, hh, 0.0f}, GetNextColor(index) });
	mesh.vertices.push_back({ {0.0f, -hh, 0.0f}, GetNextColor(index) });

	uint32_t bottomIndex = mesh.vertices.size() - 1;
	uint32_t topIndex = mesh.vertices.size() - 2;
	for (uint32_t s = 0; s < slices; ++s)
	{
		//bottom triangle
		mesh.indices.push_back(bottomIndex);
		mesh.indices.push_back(s);
		mesh.indices.push_back(s + 1);
		//top triangle
		uint32_t topRowIndex = topIndex - slices - 1 + s;
		mesh.indices.push_back(topIndex);
		mesh.indices.push_back(topRowIndex + 1);
		mesh.indices.push_back(topRowIndex);
	}

	return mesh;
}

MeshPC VEngine::Graphics::MeshBuilder::CreateSpherePC(uint32_t slices, uint32_t rings, float radius)
{
	srand(time(nullptr));
	int index = rand() % 100;

	MeshPC mesh;

	const float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	const float horzRotation = (Math::Constants::TwoPi / static_cast<float>(slices));

	for (uint32_t r = 0; r <= rings; ++r)
	{
		const float ring = static_cast<float>(r);
		const float phi = ring * vertRotation;
		for (uint32_t s = 0; s <= slices; ++s)
		{
			const float slice = static_cast<float>(s);
			const float rot = slice * horzRotation;

			mesh.vertices.push_back({ {
				radius * sin(rot) * sin(phi),
				radius * cos(phi),
				radius * cos(rot) * sin(phi)},
				GetNextColor(index)});
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX VEngine::Graphics::MeshBuilder::CreateSpherePX(uint32_t slices, uint32_t rings, float radius)
{
	MeshPX mesh;

	const float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	const float horzRotation = (Math::Constants::TwoPi / static_cast<float>(slices));
	const float uInc = 1.0f / static_cast<float>(slices);
	const float vInc = 1.0f / static_cast<float>(rings);

	for (uint32_t r = 0; r <= rings; ++r)
	{
		const float ring = static_cast<float>(r);
		const float phi = ring * vertRotation;
		for (uint32_t s = 0; s <= slices; ++s)
		{
			const float slice = static_cast<float>(s);
			const float rot = slice * horzRotation;
			const float u = 1.0f - (uInc * slice);
			const float v = vInc * ring;

			mesh.vertices.push_back({ {
				radius * sin(rot) * sin(phi),
				radius * cos(phi),
				radius * cos(rot) * sin(phi)},
				{u, v} });
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

MeshPX MeshBuilder::CreateSkySpherePX(uint32_t slices, uint32_t rings, float radius)
{
	MeshPX mesh;

	const float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	const float horzRotation = (Math::Constants::TwoPi / static_cast<float>(slices));
	const float uInc = 1.0f / static_cast<float>(slices);
	const float vInc = 1.0f / static_cast<float>(rings);

	for (uint32_t r = 0; r <= rings; ++r)
	{
		const float ring = static_cast<float>(r);
		const float phi = ring * vertRotation;
		for (uint32_t s = 0; s <= slices; ++s)
		{
			const float slice = static_cast<float>(s);
			const float rot = slice * horzRotation;
			const float u = 1.0f - (uInc * slice);
			const float v = vInc * ring;

			mesh.vertices.push_back({ {
				radius * cos(rot) * sin(phi),
				radius * cos(phi),
				radius * sin(rot) * sin(phi)},
				{u, v} });
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

Mesh MeshBuilder::CreateSphere(uint32_t slices, uint32_t rings, float radius)
{
	Mesh mesh;

	const float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	const float horzRotation = (Math::Constants::TwoPi / static_cast<float>(slices));
	const float uInc = 1.0f / static_cast<float>(slices);
	const float vInc = 1.0f / static_cast<float>(rings);

	for (uint32_t r = 0; r <= rings; ++r)
	{
		const float ring = static_cast<float>(r);
		const float phi = ring * vertRotation;
		for (uint32_t s = 0; s <= slices; ++s)
		{
			const float slice = static_cast<float>(s);
			const float rot = slice * horzRotation;
			const float u = 1.0f - (uInc * slice);
			const float v = vInc * ring;

			const float x = radius * sin(rot)* sin(phi);
			const float y = radius * cos(phi);
			const float z = radius * cos(rot) * sin(phi);

			Math::Vector3 pos = { x, y, z };
			Math::Vector3 norm = Math::Normalize(pos);
			Math::Vector3 tan = Math::Normalize({ -z, 0.0f, x });
			Math::Vector2 uvCoord = { u,v };

			mesh.vertices.push_back({pos, norm, tan, uvCoord});
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}

Mesh VEngine::Graphics::MeshBuilder::CreateSkySphere(uint32_t slices, uint32_t rings, float radius)
{
	Mesh mesh;

	const float vertRotation = (Math::Constants::Pi / static_cast<float>(rings));
	const float horzRotation = (Math::Constants::TwoPi / static_cast<float>(slices));
	const float uInc = 1.0f / static_cast<float>(slices);
	const float vInc = 1.0f / static_cast<float>(rings);

	for (uint32_t r = 0; r <= rings; ++r)
	{
		const float ring = static_cast<float>(r);
		const float phi = ring * vertRotation;
		for (uint32_t s = 0; s <= slices; ++s)
		{
			const float slice = static_cast<float>(s);
			const float rot = slice * horzRotation;
			const float u = 1.0f - (uInc * slice);
			const float v = vInc * ring;

			const float x = radius * cos(rot) * sin(phi);
			const float y = radius * cos(phi);
			const float z = radius * sin(rot)* sin(phi);

			Math::Vector3 pos = { x, y, z };
			Math::Vector3 norm = Math::Normalize(pos);
			Math::Vector3 tan = Math::Normalize({ -z, 0.0f, x });
			Math::Vector2 uvCoord = { u,v };

			mesh.vertices.push_back({ pos, norm, tan, uvCoord });
		}
	}

	CreatePlaneIndices(mesh.indices, rings, slices);

	return mesh;
}
