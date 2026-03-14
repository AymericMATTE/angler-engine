#include "pch.h"
#include "graphics.h"

// Local dependencies
#include "../buffers.h"
#include "../shaders/shader.h"
#include "../shaders/compute-shader.h"
#include "../root-signatures/compute-root-sig.h"
#include "../surfaces/render-target.h"
#include "../surfaces/depth-stencil.h"
#include "../objects/mesh.h"
#include "../particles-emitter/particles-emitter.h"

// External dependencies
#include <cassert>

namespace angler {
    GraphicsState::GraphicsState() {
        ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)))

        IDXGIAdapter* higher_performance_adapter;
        HRESULT hr = m_factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            __uuidof(IDXGIAdapter),
            (void**)&higher_performance_adapter);

        // Try to create hardware device.
        HRESULT hardwareResult = D3D12CreateDevice(
            higher_performance_adapter,                // default adapter
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&m_device)
        );


        // Fallback to WARP device.
        if (FAILED(hardwareResult)) {
            IDXGIAdapter* pWarpAdapter;
            ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

            ThrowIfFailed(D3D12CreateDevice(
                pWarpAdapter,
                D3D_FEATURE_LEVEL_12_0,
                IID_PPV_ARGS(&m_device)
            ))
        }

        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)))

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_samplerDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)))

        ThrowIfFailed(m_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&m_commandAllocator)
        ))

        ThrowIfFailed(m_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_commandAllocator,				// Associated command allocator
            nullptr,				        // Initial PipelineStateObject
            IID_PPV_ARGS(&m_commandList)
        ))

        // Start off in a closed state.  This is because the first time we refer
        // to the command list we will Reset it, and it needs to be closed before
        // calling Reset.
        m_commandList->Close();

        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
        msQualityLevels.Format = m_renderTargetFormat;
        msQualityLevels.SampleCount = 4;
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msQualityLevels.NumQualityLevels = 0;
        ThrowIfFailed(m_device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels,
            sizeof(msQualityLevels)
        ))

        m_4XMsaaQuality = msQualityLevels.NumQualityLevels;
        assert(m_4XMsaaQuality > 0 && "Unexpected MSAA quality level.");
        
        D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc;
        samplerHeapDesc.NumDescriptors = SAMPLER_MAX_COUNT;
        samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        samplerHeapDesc.NodeMask = 0;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&samplerHeapDesc,
        	IID_PPV_ARGS(&m_samplerHeap)));

        D3D12_SAMPLER_DESC wrapSamplerDesc = {};
        wrapSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        wrapSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; 
        wrapSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        wrapSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        wrapSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        wrapSamplerDesc.MipLODBias = 0.0f;
        wrapSamplerDesc.MinLOD = 0.0f;
        wrapSamplerDesc.MaxLOD = 1.0f;
        m_device->CreateSampler(&wrapSamplerDesc, m_samplerHeap->GetCPUDescriptorHandleForHeapStart());
        
        D3D12_SAMPLER_DESC clampSamplerDesc = {};
        clampSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        clampSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        clampSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        clampSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        clampSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        clampSamplerDesc.MipLODBias = 0.0f;
        clampSamplerDesc.MinLOD = 0.0f;
        clampSamplerDesc.MaxLOD = 1.0f;
        m_device->CreateSampler(&clampSamplerDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(m_samplerHeap->GetCPUDescriptorHandleForHeapStart(),1, m_samplerDescriptorSize));
        
        m_SRVHeap.init(TEXTURE_MAX_COUNT);

        ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)))
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utilsDXC)))
        ThrowIfFailed(utilsDXC->CreateDefaultIncludeHandler(&includeHandler));

        m_rangeSRV = m_SRVHeap.getDescriptorRange();

        m_rangeSampler.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        m_rangeSampler.NumDescriptors = SAMPLER_MAX_COUNT;
        m_rangeSampler.BaseShaderRegister = 0;
        m_rangeSampler.RegisterSpace = 0;
        m_rangeSampler.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    }

    GraphicsState::~GraphicsState() {
        Graphics::flushCommandQueue();

        DX12_OBJ_RELEASE(m_samplerHeap)
        DX12_OBJ_RELEASE(m_commandList)
        DX12_OBJ_RELEASE(m_commandAllocator)
        DX12_OBJ_RELEASE(m_commandQueue)
        DX12_OBJ_RELEASE(m_fence)
        DX12_OBJ_RELEASE(m_device)
        DX12_OBJ_RELEASE(m_factory)
    }

    void Graphics::drawMesh(const Mesh* _mesh, uint _count) {
        assert(_mesh != nullptr);

        if (m_state.m_isDrawingStarted == false)
            return;

        // If alpha state changed, set new PSO
        if (m_state.m_shader->isAlphaEnabled() != m_state.m_isAlphaEnabled) {
            m_state.m_commandList->SetPipelineState(m_state.m_shader->getPSO()->get());
            m_state.m_isAlphaEnabled = m_state.m_shader->isAlphaEnabled();
        }

        const D3D12_VERTEX_BUFFER_VIEW vertexBufferView = _mesh->getVertexBufferView();
        const D3D12_INDEX_BUFFER_VIEW indexBufferView = _mesh->getIndexBufferView();

        m_state.m_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
        m_state.m_commandList->IASetIndexBuffer(&indexBufferView);

        m_state.m_commandList->DrawIndexedInstanced(_mesh->getIndexCount(), _count, 0, 0, 0);
    }

    void Graphics::setShader(Shader* _shader) {
        assert(_shader != nullptr);
        assert(_shader->getRootSignature() != nullptr);
        assert(_shader->getPSO() != nullptr);

        m_state.m_shader = _shader;
        m_state.m_isAlphaEnabled = _shader->isAlphaEnabled();
        m_state.m_graphicsResourceDescList.resize(_shader->getRootSignature()->getParameterCount() - _shader->getRootSignature()->getFixedParameterCount());

        if (m_state.m_isDrawingStarted == false)
            return;

        m_state.m_commandList->SetGraphicsRootSignature(m_state.m_shader->getRootSignature()->get());
        m_state.m_commandList->SetPipelineState(m_state.m_shader->getPSO()->get());
        
        uint index = m_state.m_shader->getRootSignature()->getParameterCount() - m_state.m_shader->getRootSignature()->getFixedParameterCount() + 1;
        m_state.m_commandList->SetGraphicsRootDescriptorTable(index, m_state.m_SRVHeap.getGPUHandle());
        index++;
        m_state.m_commandList->SetGraphicsRootDescriptorTable(index, m_state.m_samplerHeap->GetGPUDescriptorHandleForHeapStart());
    }

    void Graphics::setRenderTarget(
        RenderTarget* _renderTarget, 
        bool _transitionPrevious, 
        D3D12_RESOURCE_STATES _fromState,
        D3D12_RESOURCE_STATES _toState) {
        
        assert(_renderTarget != nullptr);
        assert(_toState == D3D12_RESOURCE_STATE_RENDER_TARGET && "Target state must be RENDER_TARGET for this function");

        if (m_state.m_isDrawingStarted == false) {
            m_state.m_renderTarget = _renderTarget;
            return;
        }

        // Transition previous RT to present (if requested)
        if (m_state.m_renderTarget != nullptr && _transitionPrevious) {
            const CD3DX12_RESOURCE_BARRIER endBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
                m_state.m_renderTarget->getResource(),
                D3D12_RESOURCE_STATE_RENDER_TARGET, 
                D3D12_RESOURCE_STATE_PRESENT
            );
            m_state.m_commandList->ResourceBarrier(1, &endBarrier);
        }

        m_state.m_renderTarget = _renderTarget;
        const D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_state.m_renderTarget->getRTVCPUHandle();

        // Transition new RT from specified state to RENDER_TARGET
        const CD3DX12_RESOURCE_BARRIER startBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_state.m_renderTarget->getResource(),
            _fromState,  // Use custom from-state instead of hardcoded PRESENT
            _toState
        );
        m_state.m_commandList->ResourceBarrier(1, &startBarrier);

        m_state.m_commandList->ClearRenderTargetView(rtv, m_state.m_renderTarget->m_clearColor, 0, nullptr);

        m_state.m_commandList->RSSetViewports(1, &_renderTarget->m_viewPort);
        m_state.m_commandList->RSSetScissorRects(1, &_renderTarget->m_scissorRect);

        if (m_state.m_isDepthEnabled == false) {
            m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, nullptr);
            return;
        }

        assert(m_state.m_depthStencil != nullptr);

        const D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_state.m_depthStencil->getDepthStencilView();
        m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
    }

    void Graphics::setDepthStencil(DepthStencil* _depthStencil) {
        assert(_depthStencil != nullptr);

        m_state.m_depthStencil = _depthStencil;

        if (m_state.m_isDrawingStarted == false)
            return;

        assert(m_state.m_renderTarget != nullptr);

        const D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_state.m_renderTarget->getRTVCPUHandle();
        const D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_state.m_depthStencil->getDepthStencilView();

        m_state.m_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

        if (m_state.m_isDepthEnabled)
            m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
        else
            m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, nullptr);
    }

    void Graphics::setGraphicsBuffer(uint _index, ViewType _viewFormat , const IGraphicsBuffer* _resource) {
        assert(_resource != nullptr);

        if (_index >= m_state.m_graphicsResourceDescList.size())
            m_state.m_graphicsResourceDescList.resize(_index);

        m_state.m_graphicsResourceDescList[_index] = { _viewFormat, _resource->getResource() };

        if (m_state.m_isDrawingStarted == false)
            return;

        assert(m_state.m_shader != nullptr);

        switch (_viewFormat)
        {
        case CBV :
            m_state.m_commandList->SetGraphicsRootConstantBufferView(_index, _resource->getGPUAddress());
            break;
        case SRV :
            m_state.m_commandList->SetGraphicsRootShaderResourceView(_index, _resource->getGPUAddress());
            break;
        case UAV :
            m_state.m_commandList->SetGraphicsRootUnorderedAccessView(_index, _resource->getGPUAddress());
            break;
        case INVALID :
            assert(false && "View format is not valid.");
            break;
        }
    }
    
    void Graphics::resolveMSAAResource(ID3D12Resource* msaaSource, ID3D12Resource* dest, DXGI_FORMAT format) {
        assert(msaaSource != nullptr);
        assert(dest != nullptr);
        assert(m_state.m_commandList != nullptr);

        // Hardware resolve the resource as I understood...
        m_state.m_commandList->ResolveSubresource(
            dest, 0, msaaSource, 0, format
        );
    }

    void Graphics::setRenderTarget(RenderTarget* color, DepthStencil* depth) { // No barriers transition
        assert(color != nullptr);
        
        m_state.m_renderTarget = color;
        m_state.m_depthStencil = depth;

        if (m_state.m_isDrawingStarted == false)
            return;

        // Set the state
        const D3D12_CPU_DESCRIPTOR_HANDLE rtv = color->getRTVCPUHandle();
    
        m_state.m_commandList->RSSetViewports(1, &color->m_viewPort);
        m_state.m_commandList->RSSetScissorRects(1, &color->m_scissorRect);

        if (depth != nullptr && m_state.m_isDepthEnabled) {
            const D3D12_CPU_DESCRIPTOR_HANDLE dsv = depth->getDepthStencilView();
            m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
        } else {
            m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, nullptr);
        }
    }

    uint Graphics::getMSAAQualityLevels(uint sampleCount) {
        if (sampleCount <= 1) return 0;
        
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
        msQualityLevels.Format = m_state.m_renderTargetFormat;
        msQualityLevels.SampleCount = sampleCount;
        msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
        msQualityLevels.NumQualityLevels = 0;
        
        HRESULT hr = m_state.m_device->CheckFeatureSupport(
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &msQualityLevels,
            sizeof(msQualityLevels)
        );
        
        if (FAILED(hr)) return 0;
        return msQualityLevels.NumQualityLevels;
    }

    bool Graphics::isMSAASupported(uint sampleCount) {
        return getMSAAQualityLevels(sampleCount) > 0;
    }

    void Graphics::setComputeBuffer(uint _index, ViewType _viewFormat, IGraphicsBuffer* _resource) {
        assert(_resource != nullptr);

        if (_index >= m_state.m_computeResourceDescList.size())
            m_state.m_computeResourceDescList.resize(_index + 1);

        m_state.m_computeResourceDescList[_index] = { _viewFormat, _resource->getResource()};
    }

    void Graphics::dispatch(ComputeShader* _shader, uint _x, uint _y, uint _z) {
        assert(_shader != nullptr);

        if (m_state.m_isDrawingStarted == false)
            resetCommandList();

        m_state.m_commandList->SetComputeRootSignature(_shader->getRootSignature()->get());
        m_state.m_commandList->SetPipelineState(_shader->getPSO()->get());

        for (uint i = 0; i < _shader->getRootSignature()->getParameterCount(); ++i)
        {
            switch (m_state.m_computeResourceDescList[i].viewType)
            {
            case CBV:
                m_state.m_commandList->SetComputeRootConstantBufferView(i, m_state.m_computeResourceDescList[i].resource->GetGPUVirtualAddress());
                break;
            case SRV:
                m_state.m_commandList->SetComputeRootShaderResourceView(i, m_state.m_computeResourceDescList[i].resource->GetGPUVirtualAddress());
                break;
            case UAV:
                m_state.m_commandList->SetComputeRootUnorderedAccessView(i, m_state.m_computeResourceDescList[i].resource->GetGPUVirtualAddress());
                break;
            case INVALID:
                assert(false && "View format is not valid.");
                break;
            }
        }

        m_state.m_commandList->Dispatch(_x, _y, _z);

        if (m_state.m_isDrawingStarted)
        {
            if(m_state.m_shader != nullptr)
                return m_state.m_commandList->SetPipelineState(m_state.m_shader->getPSO()->get());

            return;
        }

        executeCommandList();
    }

    void Graphics::setPassData(IGraphicsBuffer* _buffer) {
        assert(_buffer != nullptr);

        m_state.m_passData = _buffer;

        if (m_state.m_isDrawingStarted == false)
            return;

        assert(m_state.m_shader != nullptr);

        uint index = m_state.m_shader->getRootSignature()->getParameterCount() - m_state.m_shader->getRootSignature()->getFixedParameterCount();
        m_state.m_commandList->SetGraphicsRootConstantBufferView(index, _buffer->getGPUAddress());
    }

    void Graphics::transitionGraphicsResource(IGraphicsResource* _resource, D3D12_RESOURCE_STATES _oldState, D3D12_RESOURCE_STATES _newState) {
        const CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_resource->getResource(),
    _oldState, _newState);
        m_state.m_commandList->ResourceBarrier(1, &resBarrier);
    }

    void Graphics::setDepthState(const bool _isEnabled) {
        m_state.m_isDepthEnabled = _isEnabled;

        if (m_state.m_isDrawingStarted == false)
            return;

        assert(m_state.m_renderTarget != nullptr);

        const D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_state.m_renderTarget->getRTVCPUHandle();
        if (m_state.m_isDepthEnabled == false) {
            m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, nullptr);
            return;
        }

        assert(m_state.m_depthStencil != nullptr);

        const D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_state.m_depthStencil->getDepthStencilView();
        m_state.m_commandList->OMSetRenderTargets(1, &rtv, true, &dsv);
    }

    void Graphics::compileShader(wcstr _filePath, wcstr _target, wcstr _entryPoint, IDxcResult** _result) {
        IDxcBlobEncoding* sourceBlob;
        ThrowIfFailed(m_state.utilsDXC->LoadFile(_filePath, nullptr, &sourceBlob))

        std::vector<LPCWSTR> arguments = {
                L"-E", _entryPoint,           // entry point
                L"-T", _target,       // shader model
                //DXC_ARG_PACK_MATRIX_ROW_MAJOR,
                //DXC_ARG_ALL_RESOURCES_BOUND
#ifdef _DEBUG
                L"-Qembed_debug",
                DXC_ARG_DEBUG,
                DXC_ARG_WARNINGS_ARE_ERRORS,
#else
                DXC_ARG_OPTIMIZATION_LEVEL3
#endif
        };

        DxcBuffer sourceBuffer
        {
            .Ptr = sourceBlob->GetBufferPointer(),
            .Size = sourceBlob->GetBufferSize(),
            .Encoding = 0u,
        };

        ThrowIfFailed(m_state.compiler->Compile(&sourceBuffer,
            arguments.data(),
            static_cast<uint32_t>(arguments.size()),
            m_state.includeHandler,
            IID_PPV_ARGS(_result)))

        sourceBlob->Release();
        
        HRESULT hr;
        (*_result)->GetStatus(&hr);
        if (FAILED(hr))
        {
            IDxcBlobEncoding* errorBlob;
            hr = (*_result)->GetErrorBuffer(&errorBlob);
            std::string errorMessage(
                (const char*)errorBlob->GetBufferPointer(),
                errorBlob->GetBufferSize());
            errorBlob->Release();
            throw(errorMessage);
        }
    }

    void Graphics::addTextureToHeap(Texture* _texture) {
        m_state.m_SRVHeap.addShaderResourceView(_texture);
    }

    void Graphics::addRenderTargetToHeap(RenderTarget* _renderTarget) {
        m_state.m_SRVHeap.addShaderResourceView(_renderTarget);
    }

    void Graphics::beginDrawing() {
        m_state.m_isDrawingStarted = true;

        resetCommandList();

        ID3D12DescriptorHeap* descriptor[2] = { m_state.m_SRVHeap.getHeap() , m_state.m_samplerHeap};
        m_state.m_commandList->SetDescriptorHeaps(_countof(descriptor), descriptor);

        if (m_state.m_renderTarget != nullptr) {
            const CD3DX12_RESOURCE_BARRIER startBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_state.m_renderTarget->getResource(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            m_state.m_commandList->ResourceBarrier(1, &startBarrier);
        
            m_state.m_commandList->ClearRenderTargetView(m_state.m_renderTarget->getRTVCPUHandle(), m_state.m_renderTarget->m_clearColor, 0, nullptr);

            const D3D12_CPU_DESCRIPTOR_HANDLE cpuRtDescriptor = m_state.m_renderTarget->getRTVCPUHandle();
            D3D12_CPU_DESCRIPTOR_HANDLE* dsDescriptorPtr = nullptr;

            if ( m_state.m_depthStencil != nullptr) {
                m_state.m_commandList->ClearDepthStencilView(m_state.m_depthStencil->getDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
                if (m_state.m_isDepthEnabled) {
                    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDsDescriptor = m_state.m_depthStencil->getDepthStencilView();
                    dsDescriptorPtr = &cpuDsDescriptor;
                }
            }
        
            m_state.m_commandList->OMSetRenderTargets(1, &cpuRtDescriptor, true, dsDescriptorPtr);
            m_state.m_commandList->RSSetViewports(1, &m_state.m_renderTarget->m_viewPort);
            m_state.m_commandList->RSSetScissorRects(1, &m_state.m_renderTarget->m_scissorRect);
        }

        if (m_state.m_shader != nullptr) {
            m_state.m_commandList->SetGraphicsRootSignature( m_state.m_shader->getRootSignature()->get());
            m_state.m_commandList->SetPipelineState( m_state.m_shader->getPSO()->get());
            m_state.m_commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
            uint index = 0;
            const uint parameterSize = m_state.m_shader->getRootSignature()->getParameterCount() - m_state.m_shader->getRootSignature()->getFixedParameterCount();

            for (uint i  = 0; i < parameterSize; i++) {
                switch (m_state.m_graphicsResourceDescList[i].viewType)
                {
                case CBV:
                    m_state.m_commandList->SetGraphicsRootConstantBufferView(index, m_state.m_graphicsResourceDescList[i].resource->GetGPUVirtualAddress());
                    break;
                case SRV:
                    m_state.m_commandList->SetGraphicsRootShaderResourceView(index, m_state.m_graphicsResourceDescList[i].resource->GetGPUVirtualAddress());
                    break;
                case UAV:
                    m_state.m_commandList->SetGraphicsRootUnorderedAccessView(index, m_state.m_graphicsResourceDescList[i].resource->GetGPUVirtualAddress());
                    break;
                case INVALID:
                    break;
                }
                index++;
            }

            m_state.m_commandList->SetGraphicsRootConstantBufferView(index, m_state.m_passData->getGPUAddress());
            index++;
            m_state.m_commandList->SetGraphicsRootDescriptorTable(index, m_state.m_SRVHeap.getGPUHandle());
            index++;
            m_state.m_commandList->SetGraphicsRootDescriptorTable(index, m_state.m_samplerHeap->GetGPUDescriptorHandleForHeapStart());
        }
    }

    void Graphics::endDrawing() {
        const CD3DX12_RESOURCE_BARRIER endBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_state.m_renderTarget->getResource(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_state.m_commandList->ResourceBarrier(1, &endBarrier);
        
        executeCommandList();
        m_state.m_isDrawingStarted = false;
    }

    void Graphics::resetCommandList() {
        m_state.m_commandAllocator->Reset();
        m_state.m_commandList->Reset(m_state.m_commandAllocator, nullptr);
    }

    void Graphics::executeCommandList() {
        m_state.m_commandList->Close();
        ID3D12CommandList* cmdList[] = { m_state.m_commandList };
        m_state.m_commandQueue->ExecuteCommandLists(_countof(cmdList), cmdList);

        flushCommandQueue();
    }

    D3D12_DESCRIPTOR_RANGE* Graphics::getSamplerRange() {
        return &m_state.m_rangeSampler;
    }

    D3D12_DESCRIPTOR_RANGE* Graphics::getSRVRange() {
        return &m_state.m_rangeSRV;
    }

    SRVDescriptorHeap* Graphics::getSRVHeap()
    {
        return &m_state.m_SRVHeap;
    }

    UINT Graphics::getRtvDescriptorSize() {
        return m_state.m_rtvDescriptorSize;
    }

    UINT Graphics::getDsvDescriptorSize() {
        return m_state.m_dsvDescriptorSize;
    }

    UINT Graphics::getCbvSrvUavDescriptorSize() {
        return m_state.m_cbvSrvUavDescriptorSize;
    }
    uint Graphics::getSamplerDescriptorSize() {
        return m_state.m_samplerDescriptorSize;
    }

    DXGI_FORMAT Graphics::getRenderTargetFormat() {
        return m_state.m_renderTargetFormat;
    }

    DXGI_FORMAT Graphics::getDepthStencilFormat() {
        return m_state.m_depthStencilFormat;
    }

    IDxcUtils* Graphics::GetDXCUtils()
    {
        return m_state.utilsDXC;
    }

    ID3D12Device* Graphics::getDevice() {
        return m_state.m_device;
    }

    IDXGIFactory4* Graphics::getFactory() {
        return m_state.m_factory;
    }

    ID3D12GraphicsCommandList* Graphics::getGraphicsCommandList() {
        return m_state.m_commandList;
    }

    ID3D12CommandQueue* Graphics::getCommandQueue() {
        return m_state.m_commandQueue;
    }

    void Graphics::flushCommandQueue() {
        // Advance the fence value to mark commands up to this fence point.
        m_state.m_currentFence++;

        // Add an instruction to the command queue to set a new fence point.  Because we 
        // are on the GPU timeline, the new fence point won't be set until the GPU finishes
        // processing all the commands prior to this Signal().
        ThrowIfFailed(m_state.m_commandQueue->Signal(m_state.m_fence, m_state.m_currentFence));

        // Wait until the GPU has completed commands up to this fence point.
        if (m_state.m_fence->GetCompletedValue() < m_state.m_currentFence) {
            const HANDLE eventHandle = CreateEventEx(nullptr, L"", false, EVENT_ALL_ACCESS);

            if (eventHandle == nullptr)
                throw; // TODO LOG

            // Fire event when GPU hits current fence.  
            ThrowIfFailed(m_state.m_fence->SetEventOnCompletion(m_state.m_currentFence, eventHandle));

            // Wait until the GPU hits current fence event is fired.
            WaitForSingleObject(eventHandle, INFINITE);
            CloseHandle(eventHandle);
        }
    }
}