#pragma once

// Local dependencies
#include "buffer-interface.h"

namespace angler {
    class StaticUploadBase : public IGraphicsBuffer {
    public :
        StaticUploadBase() = default;
        StaticUploadBase(const StaticUploadBase& _other) = delete;
        StaticUploadBase(StaticUploadBase&& _other) = delete;
        StaticUploadBase& operator=(const StaticUploadBase& _other) = delete;
        StaticUploadBase& operator=(StaticUploadBase&& _other) = delete;

        [[nodiscard]] ID3D12Resource* getResource() const override;
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const override;
        [[nodiscard]] uint getElementCount() const;
        [[nodiscard]] uint getElementSize() const;
    
        virtual ~StaticUploadBase() override;

    protected :
        void uploadBuffers(const void* _data, uint _bufferSize);
        void copyData() const;
    
        uint m_elementCount = 0;
        uint m_elementSize = 0;

        ID3D12Resource* m_uploadBuffer = nullptr;
        ID3D12Resource* m_resourceBuffer = nullptr;

        friend class Graphics;
    };
}