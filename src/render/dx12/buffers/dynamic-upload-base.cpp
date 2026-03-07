#include "pch.h"
#include "dynamic-upload-base.h"

// Local dependencies
#include "../graphics/graphics.h"

namespace angler {
    void DynamicUploadBase::clear() const {
        if (m_mappedData != nullptr)
            memset(m_mappedData, 0, m_bufferSize);
    }

    void DynamicUploadBase::resizeBuffer(const uint _bufferSize) {
        m_bufferSize = _bufferSize;

        if (_bufferSize <= m_bufferCapacity)
            return;

        m_bufferCapacity = _bufferSize;

        if(m_uploadBuffer) {
            m_uploadBuffer->Unmap(0, nullptr);
            m_uploadBuffer->Release();
        }

        const CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC descriptor(CD3DX12_RESOURCE_DESC::Buffer(m_bufferCapacity));

        ThrowIfFailed(Graphics::getDevice()->CreateCommittedResource(
            &heap,
            D3D12_HEAP_FLAG_NONE,
            &descriptor,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)
        ))

        ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, &m_mappedData))
    }

    ID3D12Resource* DynamicUploadBase::getResource() const {
        return m_uploadBuffer;
    }

    D3D12_GPU_VIRTUAL_ADDRESS DynamicUploadBase::getGPUAddress() const {
        return m_uploadBuffer->GetGPUVirtualAddress();
    }

    uint DynamicUploadBase::getSize() const {
        return m_bufferSize;
    }

    DynamicUploadBase::~DynamicUploadBase() {
        if(m_uploadBuffer) {
            m_uploadBuffer->Unmap(0, nullptr);
            m_uploadBuffer->Release();
        }
        m_mappedData = nullptr;
    }
}