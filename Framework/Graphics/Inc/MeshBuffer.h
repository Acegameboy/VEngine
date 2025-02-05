#pragma once

namespace VEngine::Graphics
{
	class MeshBuffer final
	{
	public:
		enum class Topology
		{
			Points,
			Lines,
			Triangles
		};

		template<class VertexType>
		void Initialize(const std::vector<VertexType>& vertices)
		{
			Initialize(vertices.data(), static_cast<uint32_t>(sizeof(VertexType)), static_cast<uint32_t>(vertices.size()));
		}
		void Initialize(const void* verticies, uint32_t vertexSize, uint32_t vertexCount);
		void Terminate();

		void SetTopology(Topology topology);

		void Render() const;

	private:
		void CreateVertexBuffer(const void* verticies, uint32_t vertexSize, uint32_t vertexCount);

		ID3D11Buffer* mVertexBuffer = nullptr;
		D3D11_PRIMITIVE_TOPOLOGY mTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		uint32_t mVertexSize = 0;
		uint32_t mVertexCount = 0;
	};
}
