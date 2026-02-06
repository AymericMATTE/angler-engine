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
private :
	Pso* pso;

	// ConstantBuffers for the next draw
	std::vector<dynamicsUploadBuffer*> constantBuffers;

	// All textures and samplers will be find here
	//GraphicsResourceManager resourceManager;
	ID3D12DescriptorHeap* samplerHeap;

	// All the data needed about the renderTarget
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	ID3D12Resource* renderTargetBuffer;
	D3D12_VIEWPORT viewPort;
	D3D12_RECT scissorRect;

	// Can be active or no when setting render target
	ID3D12DescriptorHeap* dsvHeap;
	ID3D12Resource* depthStencilBuffer;

	//Matrix given to the shader
	dynamicUploadBuffer* viewMatrix;
	dynamicUploadBuffer* projMatrix;

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

	// Dynamic Buffer To upload viewMatrix and projMatrix
	bool isDepthEnabled = false;
	bool isDrawingStarted = false;

	friend class Graphics;
};


class Graphics
{
public :
	static void DrawMesh(); // Take a Geometry* mesh, XMFLOAT4X4* world
	static void DrawMeshInstanced(); // Take a Geometry* mesh, XMFLOAT4X4* worlds
	//static void DrawText(); // TODO
	//static void DrawSprite(); // TODO

	static void SetRenderTarget(); // SetRenderTarget, Viewport and ScissorRect
	static void SetPipelineStateObject(Pso* pso); // SetPso
	static void SetConstantBuffers(int index/*, GraphicsUploadBuffer* buffer*/ ); // ConstantBufferList
	static void SetViewBuffer(dynamicUploadBuffer* matrix); // Set ViewMatrix
	static void SetProjBuffer(dynamicUploadBuffer* matrix); // Set ProjMatrix
	static void SetDepthState(bool isEnabled); // Set ProjMatrix

	static void BeginDrawing(); // Begin Drawing
	static void EndDrawing(); // End Drawing
	static void ExecuteCommandList();

	static UINT GetRtvDescriptorSize();
	static UINT GetDsvDescriptorSize();
	static UINT GetCbvSrvUavDescriptorSize();

	static ID3D12Device* GetDevice();
	static IDXGIFactory4* GetFactory();
	static ID3D12CommandList* GetCommandList();
	//static GraphicsResourceManager* GetGraphicsResourceManager();

private :
	static void AddConstantBufferToCommandList();
	static void FlushCommandQueue();
};

