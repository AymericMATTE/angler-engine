#pragma once

// Local dependencies
#include "../utils/d3dx12.h"
#include "common/types.h"

// External dependencies
#include <vector>
#include <dxgi1_4.h>
#include <dxcapi.h>

#include "../srv-descriptor-heap.h"

#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

#define SAMPLER_MAX_COUNT 2
#define TEXTURE_MAX_COUNT 64

// Forward declarations
namespace angler {
    class IGraphicsBuffer;
    class IGraphicsResource;

    class DynamicUploadBase;
    class DynamicConstantBuffer;

    class StaticUploadBase;
    class StaticUploadTexture;
    class StaticConstantBuffer;
    class ReadBackBuffer;
    
    class Shader;
    class ComputeShader;
    class PipelineStateObject;
    class DepthStencil;
    class RenderTarget;
    class Mesh;
    class ParticlesEmitter;
}

namespace angler {

    enum ViewType
    {
        INVALID,
        CBV,
        SRV,
        UAV
    };

    struct GraphicsState final {
        GraphicsState(const GraphicsState& _other) = delete;
        GraphicsState(GraphicsState&& _other) noexcept = delete;
        GraphicsState& operator=(const GraphicsState& _other) = delete;
        GraphicsState& operator=(GraphicsState&& _other) noexcept = delete;

        GraphicsState();
        ~GraphicsState();
    
    private :
        struct ResourceDesc
        {
            ViewType viewType = INVALID;
            ID3D12Resource* resource = nullptr;
        };

        Shader* m_shader = nullptr;

        IDxcCompiler3* compiler;
        IDxcUtils* utilsDXC;
        IDxcIncludeHandler* includeHandler;
    
        // ConstantBuffers for the next draw
        std::vector<ResourceDesc> m_graphicsResourceDescList = {};
        std::vector<ResourceDesc> m_computeResourceDescList = {};
    
        // All textures and samplers will be found here
        ID3D12DescriptorHeap* m_samplerHeap = nullptr;
    
        // All the data needed about the render target
        RenderTarget* m_renderTarget = nullptr;
    
        // All the data needed about the depth stencil
        DepthStencil* m_depthStencil = nullptr;
    
        // Data static for each pass
        IGraphicsBuffer* m_passData = nullptr;
    
        // DirectX12 Command Interface
        ID3D12CommandQueue* m_commandQueue;
        ID3D12CommandAllocator* m_commandAllocator;
        ID3D12GraphicsCommandList* m_commandList;
    
        // DirectX12 Interface
        ID3D12Device* m_device;
        IDXGIFactory4* m_factory;
    
        // Basic Fence
        ID3D12Fence* m_fence;
        uint m_currentFence = 0;
    
        // Constant value
        uint m_samplerDescriptorSize = 0;
        uint m_rtvDescriptorSize = 0;
        uint m_dsvDescriptorSize = 0;
        uint m_cbvSrvUavDescriptorSize = 0;
    
        D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
        DXGI_FORMAT m_renderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        SRVDescriptorHeap m_SRVHeap;

        D3D12_DESCRIPTOR_RANGE m_rangeSampler;
        D3D12_DESCRIPTOR_RANGE m_rangeSRV;

        bool m_isDepthEnabled = false; 
        bool m_isAlphaEnabled = false;
        bool m_isDrawingStarted = false;
    
        uint m_4XMsaaQuality = 0;
    
        friend class Graphics;
    };
    
    class Graphics final {
    public :
        static bool isDrawingStarted() { return m_state.m_isDrawingStarted; }
        
        static void drawMesh(const Mesh* _mesh, uint _count = 1);
    
		static void setShader(Shader* _shader); // SetPso
        static void setRenderTarget(
            RenderTarget* _renderTarget, 
            bool _transitionPrevious = true, 
            D3D12_RESOURCE_STATES _fromState = D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATES _toState = D3D12_RESOURCE_STATE_RENDER_TARGET
        ); // SetRenderTarget, Viewport and ScissorRect
        static void setRenderTarget(RenderTarget* color, DepthStencil* depth); // overload i use for MSAA
        static void setDepthStencil(DepthStencil* _depthStencil); // Set DepthStencil
        static void setPassData(IGraphicsBuffer* _buffer);

        static void setGraphicsBuffer(uint _index, ViewType _viewFormat, const IGraphicsBuffer* _resource);
        static void transitionGraphicsResource(IGraphicsResource* _resource, D3D12_RESOURCE_STATES _oldState, D3D12_RESOURCE_STATES _newState);
        static void setDepthState(bool _isEnabled); // Set DepthState
        
        // MSAA related stuff
        static void resolveMSAAResource(ID3D12Resource* msaaSource, ID3D12Resource* dest, DXGI_FORMAT format);
        static uint getMSAAQualityLevels(uint sampleCount);
        static bool isMSAASupported(uint sampleCount);

        static void setComputeBuffer(uint _index, ViewType _viewFormat, IGraphicsBuffer* _resource);
        static void dispatch(ComputeShader* _shader, uint _x, uint _y, uint _z);

        static void compileShader(wcstr _filePath, wcstr _target, wcstr _entryPoint, IDxcResult** _result);

        static void addTextureToHeap(Texture* _texture);
        static void addRenderTargetToHeap(RenderTarget* _renderTarget);
    
        static void beginDrawing(); // Begin Drawing
        static void endDrawing(); // End Drawing
        
        static void resetCommandList();
        static void executeCommandList();
    
        static D3D12_DESCRIPTOR_RANGE* getSamplerRange();
        static D3D12_DESCRIPTOR_RANGE* getSRVRange();

        static SRVDescriptorHeap* getSRVHeap();

        static uint getRtvDescriptorSize();
        static uint getDsvDescriptorSize();
        static uint getCbvSrvUavDescriptorSize();
        static uint getSamplerDescriptorSize();
    
        static DXGI_FORMAT getRenderTargetFormat();
        static DXGI_FORMAT getDepthStencilFormat();
    
        static IDxcUtils* GetDXCUtils();
        static ID3D12Device* getDevice();
        static IDXGIFactory4* getFactory();
        static ID3D12GraphicsCommandList* getGraphicsCommandList();
        static ID3D12CommandQueue* getCommandQueue();
    
        static void flushCommandQueue();
        
    private :
        static inline GraphicsState m_state = GraphicsState();
    };
}
