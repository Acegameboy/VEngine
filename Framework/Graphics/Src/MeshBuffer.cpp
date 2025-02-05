#include "Precompiled.h"
#include "MeshBuffer.h"
#include "GraphicsSystem.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void MeshBuffer::Initialize(const void* verticies, uint32_t vertexSize, uint32_t vertexCount)
{
	CreateVertexBuffer(verticies, vertexSize, vertexCount);
}

void MeshBuffer::Terminate()
{
	SafeRelease(mVertexBuffer);
}

void MeshBuffer::SetTopology(Topology topology)
{
	switch (topology)
	{
	case Topology::Points:
	{
		mTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	}
	break;
	case Topology::Lines:
	{
		mTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	}
	break;
	case Topology::Triangles:
	{
		mTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
	break;
	default:
		break;
	}
}

void MeshBuffer::Render() const
{
	auto context = GraphicsSystem::Get()->GetContext();

	context->IASetPrimitiveTopology(mTopology);

	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexSize, &offset);
	context->Draw((UINT)mVertexCount, 0);
}

void MeshBuffer::CreateVertexBuffer(const void* verticies, uint32_t vertexSize, uint32_t vertexCount)
{
	mVertexCount = vertexCount;
	mVertexSize = vertexSize;

	auto device = GraphicsSystem::Get()->GetDevice();

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(vertexCount * vertexSize);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = verticies;

	HRESULT hr = device->CreateBuffer(&desc, &initData, &mVertexBuffer);
	ASSERT(SUCCEEDED(hr), "MeshBuffer: failed to create vertex buffer");
}
