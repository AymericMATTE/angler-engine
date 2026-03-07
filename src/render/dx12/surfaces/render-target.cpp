#include "pch.h"
#include "render-target.h"

// Local dependencies
#include <cassert>

#include "../graphics/graphics.h"

namespace angler {
    RenderTarget::~RenderTarget() {
        DX12_OBJ_RELEASE(m_buffer)
        DX12_OBJ_RELEASE(m_viewHeap)
    }

    void RenderTarget::initialize(const uint _width, const uint _height, float _clearColor[4]) {
        // No MSAA
        m_sampleCount = 1;
        m_sampleQuality = 0;
        
        m_viewPort.Width = static_cast<float>(_width);
        m_viewPort.Height = static_cast<float>(_height);
        m_viewPort.TopLeftX = 0;
        m_viewPort.TopLeftY = 0;
        m_viewPort.MinDepth = 0.0f;
        m_viewPort.MaxDepth = 1.0f;

        m_scissorRect = { 0, 0, static_cast<long>(_width), static_cast<long>(_height) };

        m_desc = {};
        m_desc.Format = Graphics::getRenderTargetFormat();
        m_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        m_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        m_desc.Texture2D.MipLevels = 1;
        m_desc.Texture2D.MostDetailedMip = 0;

		ID3D12Device* d3dDevice = Graphics::getDevice();

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = 1;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        ThrowIfFailed(d3dDevice->CreateDescriptorHeap(
            &rtvHeapDesc, IID_PPV_ARGS(&m_viewHeap)
        ))
        
        // Clear value
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = Graphics::getRenderTargetFormat();
        if (_clearColor) {
            clearValue.Color[0] = _clearColor[0];
            clearValue.Color[1] = _clearColor[1];
            clearValue.Color[2] = _clearColor[2];
            clearValue.Color[3] = _clearColor[3];
            m_clearColor[0] = _clearColor[0];
            m_clearColor[1] = _clearColor[1];
            m_clearColor[2] = _clearColor[2];
            m_clearColor[3] = _clearColor[3];
        } else {
            clearValue.Color[0] = 0.0f;
            clearValue.Color[1] = 0.0f;
            clearValue.Color[2] = 0.0f;
            clearValue.Color[3] = 1.0f;
        }

        CD3DX12_HEAP_PROPERTIES heapCommon(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC rtvDesc = CD3DX12_RESOURCE_DESC::Tex2D(Graphics::getRenderTargetFormat(), _width, _height);
        rtvDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        CD3DX12_CPU_DESCRIPTOR_HANDLE viewHeapHandle(m_viewHeap->GetCPUDescriptorHandleForHeapStart());
        ThrowIfFailed(d3dDevice->CreateCommittedResource(
            &heapCommon,
            D3D12_HEAP_FLAG_NONE,
            &rtvDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(&m_buffer)
        ))

        d3dDevice->CreateRenderTargetView(m_buffer, nullptr, viewHeapHandle);
	}

    void RenderTarget::initializeMSAA(uint _width, uint _height, uint _sampleCount, uint _quality, float _clearColor[4]) {
        assert(_sampleCount > 1 && "MSAA sample count must be greater than 1");
        
        m_sampleCount = _sampleCount;
        m_sampleQuality = _quality;
        
        m_viewPort.Width = static_cast<float>(_width);
        m_viewPort.Height = static_cast<float>(_height);
        m_viewPort.TopLeftX = 0;
        m_viewPort.TopLeftY = 0;
        m_viewPort.MinDepth = 0.0f;
        m_viewPort.MaxDepth = 1.0f;

        m_scissorRect = { 0, 0, static_cast<long>(_width), static_cast<long>(_height) };

        ID3D12Device* d3dDevice = Graphics::getDevice();

        // Create RTV descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = 1;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        ThrowIfFailed(d3dDevice->CreateDescriptorHeap(
            &rtvHeapDesc, IID_PPV_ARGS(&m_viewHeap)
        ))

        // MSAA resource descriptor (I still don't get the utility of this desc... but anyways...)
        CD3DX12_RESOURCE_DESC msaaDesc = CD3DX12_RESOURCE_DESC::Tex2D(Graphics::getRenderTargetFormat(),
            _width, _height, 1, 1, _sampleCount, _quality);
        msaaDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        
        // Clear value
        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = Graphics::getRenderTargetFormat();
        if (_clearColor) {
            clearValue.Color[0] = _clearColor[0];
            clearValue.Color[1] = _clearColor[1];
            clearValue.Color[2] = _clearColor[2];
            clearValue.Color[3] = _clearColor[3];
            m_clearColor[0] = _clearColor[0];
            m_clearColor[1] = _clearColor[1];
            m_clearColor[2] = _clearColor[2];
            m_clearColor[3] = _clearColor[3];
        } else {
            clearValue.Color[0] = 0.0f;
            clearValue.Color[1] = 0.0f;
            clearValue.Color[2] = 0.0f;
            clearValue.Color[3] = 1.0f;
        }

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_CPU_DESCRIPTOR_HANDLE viewHeapHandle(m_viewHeap->GetCPUDescriptorHandleForHeapStart());
        
        ThrowIfFailed(d3dDevice->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &msaaDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(&m_buffer)
        ))

        // Create MSAA RTV
        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = Graphics::getRenderTargetFormat();
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        
        d3dDevice->CreateRenderTargetView(m_buffer, &rtvDesc, viewHeapHandle);

        // MSAA textures cannot be bound as SRV directly, they must be resolved first
        // So I only store the desc for the resolve target format reference
        m_desc = {};
        m_desc.Format = Graphics::getRenderTargetFormat();
        m_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS; // This is for ref only
        m_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        m_desc.Texture2D.MipLevels = 1;
        m_desc.Texture2D.MostDetailedMip = 0;
    }

    void RenderTarget::initialize(ID3D12Resource* _resource, const uint _width, const uint _height, float _clearColor[4]) {
        _resource->AddRef();

        // Check if the resource is MSAA
        D3D12_RESOURCE_DESC desc = _resource->GetDesc();
        m_sampleCount = desc.SampleDesc.Count;
        m_sampleQuality = desc.SampleDesc.Quality;

        m_viewPort.Width = static_cast<float>(_width);
        m_viewPort.Height = static_cast<float>(_height);
        m_viewPort.TopLeftX = 0;
        m_viewPort.TopLeftY = 0;
        m_viewPort.MinDepth = 0.0f;
        m_viewPort.MaxDepth = 1.0f;

        m_scissorRect = { 0, 0, static_cast<long>(_width), static_cast<long>(_height) };

        ID3D12Device* d3dDevice = Graphics::getDevice();

        m_desc = {};
        m_desc.Format = Graphics::getRenderTargetFormat();
        m_desc.ViewDimension = (m_sampleCount > 1) ? D3D12_SRV_DIMENSION_TEXTURE2DMS : D3D12_SRV_DIMENSION_TEXTURE2D;
        m_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        m_desc.Texture2D.MipLevels = 1;
        m_desc.Texture2D.MostDetailedMip = 0;

        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
        rtvHeapDesc.NumDescriptors = 1;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        rtvHeapDesc.NodeMask = 0;
        d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_viewHeap));

        CD3DX12_CPU_DESCRIPTOR_HANDLE viewHeapHandle(m_viewHeap->GetCPUDescriptorHandleForHeapStart());
        m_buffer = _resource;

        // Create RTV depending on if the resource is MSAA or not
        if (m_sampleCount > 1) {
            D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
            rtvDesc.Format = Graphics::getRenderTargetFormat();
            rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
            d3dDevice->CreateRenderTargetView(m_buffer, &rtvDesc, viewHeapHandle);
        } else {
            d3dDevice->CreateRenderTargetView(m_buffer, nullptr, viewHeapHandle);
        }

        if (_clearColor == nullptr)
            return;

        m_clearColor[0] = _clearColor[0];
        m_clearColor[1] = _clearColor[1];
        m_clearColor[2] = _clearColor[2];
        m_clearColor[3] = _clearColor[3];
    }

    ID3D12Resource* RenderTarget::getResource() const {
        return m_buffer;
    }

    D3D12_GPU_VIRTUAL_ADDRESS RenderTarget::getGPUAddress() const {
        return m_buffer->GetGPUVirtualAddress();
    }

    uint RenderTarget::getWidth() const {
        return static_cast<uint>(m_viewPort.Width);
    }

    uint RenderTarget::getHeight() const {
        return static_cast<uint>(m_viewPort.Height);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget::getRTVCPUHandle() const {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(
            m_viewHeap->GetCPUDescriptorHandleForHeapStart(),
            0,
            Graphics::getRtvDescriptorSize()
        );
    }

    D3D12_GPU_DESCRIPTOR_HANDLE RenderTarget::getRTVGPUHandle() const {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(
            m_viewHeap->GetGPUDescriptorHandleForHeapStart(),
            0,
            Graphics::getRtvDescriptorSize()
        );
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC RenderTarget::getSRVDesc() const {
        return m_desc;
    }
    
    bool RenderTarget::isMSAA() const {
        return m_sampleCount > 1;
    }
    
    uint RenderTarget::getSampleCount() const {
        return m_sampleCount;
    }
    
    uint RenderTarget::getSampleQuality() const {
        return m_sampleQuality;
    }
}