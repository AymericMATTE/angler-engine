#pragma once

// Local dependencies
#include "common/types.h"

// External dependencies
#include <d3d12.h>

// Forward declarations
namespace angler {
    class DynamicUploadBase;
    class StaticUploadBase;
}

namespace angler {
    class ReadBackBuffer final {
    public:
        ReadBackBuffer() = default;
    
        ReadBackBuffer(const ReadBackBuffer& _other) = delete;
        ReadBackBuffer(ReadBackBuffer&& _other) = delete;
        ReadBackBuffer& operator=(const ReadBackBuffer& _other) = delete;
        ReadBackBuffer& operator=(ReadBackBuffer&& _other) = delete;

        void readback(const StaticUploadBase* _upload);
        void readback(const DynamicUploadBase* _upload);
    
        [[nodiscard]] const char* getData() const;
        [[nodiscard]] uint getSize() const;
    
        ~ReadBackBuffer();

    private:
        void setReadbackBufferSize(uint _dataSize);
        void copyData(ID3D12Resource* _sourceBuffer, uint _bufferSize, D3D12_RESOURCE_STATES _sourceInitialState);
        void map();
        void release();
    
        ID3D12Resource* m_readbackBuffer = nullptr;
        char* m_mappedData = nullptr;
        uint m_dataSize = 0;
        uint m_dataCapacity = 0;

        friend class Graphics;
    };
}