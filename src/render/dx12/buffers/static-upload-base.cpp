#include "pch.h"
#include "static-upload-base.h"

#include <cassert>

#include "../graphics/graphics.h"

namespace angler {
    void StaticUploadBase::uploadBuffers(const void* _data, const uint _bufferSize) {
        assert(Graphics::isDrawingStarted() == false);

        const CD3DX12_HEAP_PROPERTIES heapUpload(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC descriptor(CD3DX12_RESOURCE_DESC::Buffer(_bufferSize));

        ThrowIfFailed(Graphics::getDevice()->CreateCommittedResource(
            &heapUpload,
            D3D12_HEAP_FLAG_NONE,
            &descriptor,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)))

        const CD3DX12_HEAP_PROPERTIES heapCommon(D3D12_HEAP_TYPE_DEFAULT);

        ThrowIfFailed(Graphics::getDevice()->CreateCommittedResource(
            &heapCommon,
            D3D12_HEAP_FLAG_NONE,
            &descriptor,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_resourceBuffer)))

        void* mappedPtr;
        ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, &mappedPtr))
        memcpy(mappedPtr, _data, m_elementCount * m_elementSize);
        m_uploadBuffer->Unmap(0, nullptr);

        Graphics::resetCommandList();
        copyData();
        Graphics::executeCommandList();
        m_uploadBuffer->Release();
        m_uploadBuffer = nullptr;
    }

    void StaticUploadBase::copyData() const {
        ID3D12GraphicsCommandList* commandList = Graphics::getGraphicsCommandList();
        commandList->CopyResource(m_resourceBuffer, m_uploadBuffer);
        const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resourceBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
        commandList->ResourceBarrier(1, &barrier);
    }

    ID3D12Resource* StaticUploadBase::getResource() const {
        return m_resourceBuffer;
    }

    D3D12_GPU_VIRTUAL_ADDRESS StaticUploadBase::getGPUAddress() const {
        return m_resourceBuffer->GetGPUVirtualAddress();
    }

    uint StaticUploadBase::getElementCount() const {
        return m_elementCount;
    }

    uint StaticUploadBase::getElementSize() const {
        return m_elementSize;
    }

    StaticUploadBase::~StaticUploadBase() {
        DX12_OBJ_RELEASE(m_resourceBuffer)
    }
}