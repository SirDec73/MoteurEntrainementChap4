#pragma once
#include <string>
#include "../../Common/d3dUtil.h"

//ping
class Shader
{
public:
    Shader(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    ID3DBlob* CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target);

    ID3D12RootSignature* mRootSignature;
    ID3D12PipelineState* mPSO;
};
