#include "pch.h"
#include "readback-buffer.h"

#include <cassert>

#include "static-upload-base.h"
#include "dynamic-upload-base.h"
#include "../graphics/graphics.h"

namespace angler {
    
    void ReadBackBuffer::readback(const StaticUploadBase* _upload) {

        assert(Graphics::isDrawingStarted() == false);

        Graphics::resetCommandList();
        copyData(_upload->getResource(), _upload->getSize(), D3D12_RESOURCE_STATE_COMMON);
        Graphics::executeCommandList();
        map();
    }
    
    void ReadBackBuffer::readback(const DynamicUploadBase* _upload) {

        assert(Graphics::isDrawingStarted() == false);

        Graphics::resetCommandList();
        copyData(_upload->getResource(), _upload->getSize(), D3D12_RESOURCE_STATE_GENERIC_READ);
        Graphics::executeCommandList();
        map();
    }

    const char* ReadBackBuffer::getData() const {
        return m_mappedData;
    }

    uint ReadBackBuffer::getSize() const {
        return m_dataSize;
    }

    void ReadBackBuffer::setReadbackBufferSize(const uint _dataSize) {
        m_dataSize = _dataSize;
    
        if (_dataSize <= m_dataCapacity)
            return;

        release();
        m_dataCapacity = _dataSize;
    
        const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_READBACK);
        const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_dataSize);

        ThrowIfFailed(Graphics::getDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_readbackBuffer)))
    }

    void ReadBackBuffer::copyData(ID3D12Resource* _sourceBuffer, const uint _bufferSize, const D3D12_RESOURCE_STATES _sourceInitialState) {
        setReadbackBufferSize(_bufferSize);
        
        ID3D12GraphicsCommandList* cmdList = Graphics::getGraphicsCommandList();
    
        if (_sourceInitialState != D3D12_RESOURCE_STATE_COPY_SOURCE) {
            const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition( _sourceBuffer, _sourceInitialState, D3D12_RESOURCE_STATE_COPY_SOURCE);
            cmdList->ResourceBarrier(1, &barrier);
        }
    
        cmdList->CopyBufferRegion( m_readbackBuffer, 0, _sourceBuffer, 0, m_dataSize);
    
        if (_sourceInitialState != D3D12_RESOURCE_STATE_COPY_SOURCE) {
            const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(_sourceBuffer,D3D12_RESOURCE_STATE_COPY_SOURCE,_sourceInitialState);
            cmdList->ResourceBarrier(1, &barrier);
        }
    }

    void ReadBackBuffer::release() {
        if (m_readbackBuffer) {
            m_readbackBuffer->Unmap(0, nullptr);
            m_mappedData = nullptr;
            m_readbackBuffer->Release();
        }
    }

    void ReadBackBuffer::map() {
        const D3D12_RANGE readRange{ 0, m_dataSize };
        ThrowIfFailed(m_readbackBuffer->Map( 0, &readRange, (void**)&m_mappedData))
    }

    ReadBackBuffer::~ReadBackBuffer() {
        release();
    }
}