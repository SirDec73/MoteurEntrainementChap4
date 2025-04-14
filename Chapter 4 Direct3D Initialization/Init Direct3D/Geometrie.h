#pragma once
#pragma once

#include "../../Common/d3dUtil.h"

struct Vertex
{
    Vertex() {}
    Vertex(
        const DirectX::XMFLOAT3& p,
        const DirectX::XMFLOAT4& c) :
        Position(p),
        Color(c)
    {}
    Vertex(
        float px, float py, float pz,
        float r, float g, float b) :
        Position(px, py, pz),
        Color(r, g, b, 1) {}

    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT4 Color;
};

using uint16 = std::uint16_t;
using uint32 = std::uint32_t;

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<uint32> Indices32;

    std::vector<uint16>& GetIndices16()
    {
        if (mIndices16.empty())
        {
            mIndices16.resize(Indices32.size());
            for (size_t i = 0; i < Indices32.size(); ++i)
                mIndices16[i] = static_cast<uint16>(Indices32[i]);
        }

        return mIndices16;
    }

private:
    std::vector<uint16> mIndices16;
};

class Geometrie
{

public:

    void CreateCube(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float width, float height, float depth);

    // Coter CPU
    MeshData meshData;
    ID3DBlob* VertexBuffer;
    ID3DBlob* IndexBuffer;

    // Coter GPU
    ID3D12Resource* VertexBufferGPU = nullptr;
    ID3D12Resource* IndexBufferGPU = nullptr;

    // Uploader
    ID3D12Resource* VertexBufferUploader = nullptr;
    ID3D12Resource* IndexBufferUploader = nullptr;

    // Les donnees pour dessiner a l'eran
    UINT VertexByteStride = 0;
    UINT VertexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    UINT IndexBufferByteSize = 0;

    UINT IndicesCount = 0;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }
};


