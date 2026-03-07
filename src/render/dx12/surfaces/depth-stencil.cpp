#include "pch.h"
#include "depth-stencil.h"

#include <cassert>

#include "../graphics/graphics.h"

namespace angler {
    DepthStencil::~DepthStencil() {
        DX12_OBJ_RELEASE(m_viewHeap)
        DX12_OBJ_RELEASE(m_buffer)
    }

    void DepthStencil::initialize(const uint _width, const uint _height, 
        const uint _sampleCount, const uint _sampleQuality,
        const D3D12_RESOURCE_DIMENSION _dimension,
        const uint _mipLevels) {
        
        m_sampleCount = _sampleCount;
        m_sampleQuality = _sampleQuality;
        const bool msaaState = (_sampleCount > 1);

        ID3D12Device* d3dDevice = Graphics::getDevice();
        
        D3D12_RESOURCE_DESC depthStencilDesc = {};
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Dimension = _dimension;
        depthStencilDesc.Width = _width;    
        depthStencilDesc.Height = _height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = _mipLevels;
        depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthStencilDesc.SampleDesc.Count = _sampleCount;
        depthStencilDesc.SampleDesc.Quality = msaaState ? (_sampleQuality - 1) : 0;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = Graphics::getDepthStencilFormat();
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;    

        CD3DX12_HEAP_PROPERTIES heapProperty(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(d3dDevice->CreateCommittedResource(
            &heapProperty,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_COMMON,
            &clearValue,
            IID_PPV_ARGS(&m_buffer)))

        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 1;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_viewHeap)));

        // Create DSV
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.Format = Graphics::getDepthStencilFormat();
        
        if (msaaState) {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
            // Texture2DMS apparently needs nothing else
        } else {
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Texture2D.MipSlice = 0;
        }
        
        d3dDevice->CreateDepthStencilView(m_buffer, &dsvDesc, m_viewHeap->GetCPUDescriptorHandleForHeapStart());

        // Initial resource barrier
        Graphics::resetCommandList();
        CD3DX12_RESOURCE_BARRIER rb = CD3DX12_RESOURCE_BARRIER::Transition(
            m_buffer, 
            D3D12_RESOURCE_STATE_COMMON, 
            D3D12_RESOURCE_STATE_DEPTH_WRITE
        );
        Graphics::getGraphicsCommandList()->ResourceBarrier(1, &rb);
        Graphics::executeCommandList();
    }

    D3D12_GPU_VIRTUAL_ADDRESS DepthStencil::getGPUAddress() const {
        return m_buffer->GetGPUVirtualAddress();
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE DepthStencil::getDepthStencilView() const {
        return static_cast<CD3DX12_CPU_DESCRIPTOR_HANDLE>(m_viewHeap->GetCPUDescriptorHandleForHeapStart());
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE DepthStencil::getGPUDescriptorHandle() const {
        return static_cast<CD3DX12_GPU_DESCRIPTOR_HANDLE>(m_viewHeap->GetGPUDescriptorHandleForHeapStart());
    }
}