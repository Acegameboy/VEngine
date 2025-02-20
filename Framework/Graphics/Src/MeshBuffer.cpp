#include "Precompiled.h"
#include "MeshBuffer.h"
#include "GraphicsSystem.h"

using namespace VEngine;
using namespace VEngine::Graphics;

void MeshBuffer::Initialize(const void* verticies, uint32_t vertexSize, uint32_t vertexCount)
{
	CreateVertexBuffer(verticies, vertexSize, vertexCount);
}

void VEngine::Graphics::MeshBuffer::Initialize(const void* verticies, uint32_t vertexSize, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount)
{
	CreateVertexBuffer(verticies, vertexSize, vertexCount);
	CreateIndexBuffer(indices, indexCount);
}

void MeshBuffer::Terminate()
{
	SafeRelease(mVertexBuffer);
	SafeRelease(mIndexBuffer);
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

void MeshBuffer::Update(const void* vertices, uint32_t vertexCount)
{
	mVertexCount = vertexCount;
	auto context = GraphicsSystem::Get()->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, vertices, (vertexCount * mVertexSize));
	context->Unmap(mVertexBuffer, 0);
}

void MeshBuffer::Render() const
{
	auto context = GraphicsSystem::Get()->GetContext();

	context->IASetPrimitiveTopology(mTopology);

	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexSize, &offset);
	if (mIndexBuffer != nullptr)
	{
		context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(mIndexCount, 0, 0);
	}
	else
	{
		context->Draw((UINT)mVertexCount, 0);
	}
}

void MeshBuffer::CreateVertexBuffer(const void* verticies, uint32_t vertexSize, uint32_t vertexCount)
{
	mVertexCount = vertexCount;
	mVertexSize = vertexSize;

	auto device = GraphicsSystem::Get()->GetDevice();

	const bool isDynamic = (verticies == nullptr);
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(vertexCount * vertexSize);
	desc.Usage = isDynamic? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = isDynamic ? D3D10_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = verticies;

	HRESULT hr = device->CreateBuffer(&desc, (isDynamic? nullptr : &initData), &mVertexBuffer);
	ASSERT(SUCCEEDED(hr), "MeshBuffer: failed to create vertex buffer");
}

void VEngine::Graphics::MeshBuffer::CreateIndexBuffer(const uint32_t* indices, uint32_t indexCount)
{
	if (indexCount == 0)
	{
		return;
	}

	mIndexCount = indexCount;

	auto device = GraphicsSystem::Get()->GetDevice();

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(indexCount * sizeof(uint32_t));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = indices;

	HRESULT hr = device->CreateBuffer(&desc, &initData, &mIndexBuffer);
	ASSERT(SUCCEEDED(hr), "MeshBuffer: failed to create vertex buffer");

}
