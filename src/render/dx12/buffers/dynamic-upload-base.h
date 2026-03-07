#pragma once

// Local dependencies
#include "common/types.h"
#include "buffer-interface.h"

namespace angler {
    class DynamicUploadBase : public IGraphicsBuffer {
    public:
        DynamicUploadBase() = default;
        DynamicUploadBase(const DynamicUploadBase& _other) = delete;
        DynamicUploadBase(DynamicUploadBase&& _other) = delete;
        DynamicUploadBase& operator=(DynamicUploadBase&& _other) = delete;
        DynamicUploadBase& operator=(const DynamicUploadBase& _other) = delete;

        [[nodiscard]] ID3D12Resource* getResource() const override;
        [[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS getGPUAddress() const override;
        [[nodiscard]] uint getSize() const override;

        template<typename T>
        void copyData(const T* _data, uint _index, uint _size = sizeof(T));
        void clear() const;

        virtual void init(uint _bufferSize) = 0;

        virtual ~DynamicUploadBase() override;

    protected:
        void resizeBuffer(uint _bufferSize);

        uint m_bufferSize = 0;
        uint m_bufferCapacity = 0;
        ID3D12Resource* m_uploadBuffer = nullptr;
        void* m_mappedData = nullptr;

        friend class Graphics;
    };
}

#include "dynamic-upload-base.inl"