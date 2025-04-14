//***************************************************************************************
// Init Direct3D.cpp by Frank Luna (C) 2015 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************

#include "../../Common/d3dApp.h"
#include <DirectXColors.h>
#include "Shader.h"

#include "UploadBuffer.h"
#include "Transform.h"
#include "Geometrie.h"

using namespace DirectX;

class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	virtual bool Initialize()override;

	Geometrie* geo;
	Transform transform;

private:
    virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override {};
	//ping
	void BeginDraw();
    virtual void Draw(Shader&, Geometrie&, UploadBuffer<ObjectData>*);
	void EndDraw();



};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    try
    {
        InitDirect3DApp theApp(hInstance);
        if(!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch(DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
: D3DApp(hInstance) 
{
}

InitDirect3DApp::~InitDirect3DApp()
{
}

bool InitDirect3DApp::Initialize()
{
    if(!D3DApp::Initialize())
		return false;
		
	//ping
	XMMATRIX ProjMatrix = XMMatrixPerspectiveFovLH(0.25f * 3.14f, AspectRatio(), 0.1f, 500);
	XMStoreFloat4x4(&mProj, ProjMatrix);

	mDirectCmdListAlloc->Reset();
	mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

	//CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
	//	D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//mCommandList->ResourceBarrier(1, &barrier);

	geo = new Geometrie();
	geo->CreateCube(md3dDevice.Get(), mCommandList.Get(), 1.0f, 1.0f, 1.0f);

	mCommandList->Close();
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get()};
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	transform.Identity();
	transform.SetPosition(0.0f, 0.0f, 0.0f);
	transform.UpdateMatrix();

	return true;
}

void InitDirect3DApp::OnResize()
{
	D3DApp::OnResize();
}

void InitDirect3DApp::Update(const GameTimer& gt)
{
	//ping
	
	Transform camTransform;
	camTransform.Identity();
	camTransform.SetPosition(2.0f, 2.0f, -5.0f);
	camTransform.UpdateMatrix();

	XMStoreFloat4x4(&mView, XMMatrixInverse(nullptr,camTransform.GetMatrix()));

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	GlobalInformation info;
	XMStoreFloat4x4(&info.ViewProj, XMMatrixTranspose(viewProj));
	mGlobalConstantBuffer->CopyData(0, info);

	UploadBuffer<ObjectData>* constantBuffer = new UploadBuffer<ObjectData>(md3dDevice.Get(), 1, true);
	constantBuffer->Resource()->SetName(L"PASS_BUFFER");

	UploadBuffer<ObjectData>* constantBuffer2 = new UploadBuffer<ObjectData>(md3dDevice.Get(), 1, true);
	constantBuffer->Resource()->SetName(L"PASS_BUFFER");

	if (GetAsyncKeyState('Z') & 0x8000) {

		transform.RotatePitch(50 * gt.DeltaTime());
		transform.UpdateMatrix();
	}

	Transform transform2;
	transform2.Identity();
	transform2.SetPosition(3.0f,0.0f, 0.0f);
	transform2.UpdateMatrix();

	ObjectData objConstants;
	DirectX::XMStoreFloat4x4(&objConstants.world, DirectX::XMMatrixTranspose(transform.GetMatrix()));
	constantBuffer->CopyData(0, objConstants);

	DirectX::XMStoreFloat4x4(&objConstants.world, DirectX::XMMatrixTranspose(transform2.GetMatrix()));
	constantBuffer2->CopyData(0, objConstants);

	BeginDraw();
	Draw(*mShader, *geo, constantBuffer);
	Draw(*mShader, *geo, constantBuffer2);
	EndDraw();

}

//ping

void InitDirect3DApp::BeginDraw() {
	mDirectCmdListAlloc->Reset();
	mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	CD3DX12_CPU_DESCRIPTOR_HANDLE currentBackBufferView(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = DepthStencilView();

	mCommandList->ClearRenderTargetView(currentBackBufferView, DirectX::Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &currentBackBufferView, true, &depthStencilView);
}

void InitDirect3DApp::Draw(Shader& shader, Geometrie& geo, UploadBuffer<ObjectData>* buffer)
{
    

	//ping
	
	mCommandList->SetGraphicsRootSignature(shader.mRootSignature);
	mCommandList->SetPipelineState(shader.mPSO);

	mCommandList->SetGraphicsRootConstantBufferView(0, buffer->Resource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootConstantBufferView(1, mGlobalConstantBuffer->Resource()->GetGPUVirtualAddress());
	
	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = geo.VertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBuffer = geo.IndexBufferView();

	mCommandList->IASetVertexBuffers(0, 1, &vertexBuffer);
	mCommandList->IASetIndexBuffer(&indexBuffer);
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mCommandList->DrawIndexedInstanced(geo.IndicesCount, 1, 0, 0, 0);    
}
void InitDirect3DApp::EndDraw() {
	// Indicate a state transition on the resource usage.
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	mCommandList->ResourceBarrier(1, &barrier);
	// Done recording commands.
	mCommandList->Close();

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	mSwapChain->Present(0, 0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}
