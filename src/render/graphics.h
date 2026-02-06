#pragma once
#include <d3dx12.h>
#include <dxgi1_4.h>
#include "DirectXMath.h"
#include <vector>

#undef DrawText

struct Pso
{
	ID3D12PipelineState* PSO;
	ID3D12RootSignature* RootSignature;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
};

struct GraphicsState
{
	Pso& pso;

	// TODO : Add ConstantBuffers

	// All texture will be find here
	ID3D12DescriptorHeap* textureHeap;

	// All the data needed about the renderTarget
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	ID3D12Resource* renderTargetBuffer;
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;

	// Can be active or no when setting render target
	ID3D12DescriptorHeap* dsvHeap;
	ID3D12Resource* depthStencilBuffer;

	//Matrix given to the shader
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	// DirectX12 Command Interface
	ID3D12CommandQueue* commandQueue;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12CommandList* commandList;

	// DirectX12 Interface
	ID3D12Device* device;
	IDXGIFactory4* factory;

	// Basic Fence
	ID3D12Fence* fence;

	// Constant value
	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	GraphicsState();

	~GraphicsState();
private :
	// Dynamic Buffer To upload viewMatrix and projMatrix
};



class Graphics
{
public :
	static void DrawMesh(); // Take a Mesh* mesh, XMFLOAT4X4* world
	static void DrawMeshInstanced(); // Take a Mesh* mesh, XMFLOAT4X4* worlds
	//static void DrawText(); // TODO
	//static void DrawSprite(); // TODO
	static void SetRenderTarget(); // SetRenderTarget, Viewport and ScissorRect
	static void SetPipelineStateObject(); // SetPso
	static void SetConstantBuffers(int index/*, GraphicsBuffer(->UploadBuffer)* buffer */ ); // ConstantBufferList
	static void SetView(DirectX::XMFLOAT4X4 matrix); // Set ViewMatrix
	static void SetProj(DirectX::XMFLOAT4X4); // Set ProjMatrix
	static void BeginDrawing(); // Begin Drawing with actual parameters
	static void EndDrawing(); // End Drawing in global
private :
	static void FlushCommandQueue();
};

