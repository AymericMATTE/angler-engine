#pragma once

// Local dependencies
#include "dynamic-upload-base.h"

namespace angler {
    class DynamicUploadBuffer final : public DynamicUploadBase {
    public:
        DynamicUploadBuffer() = default;
        explicit DynamicUploadBuffer(uint _bufferSize);

        void init(uint _bufferSize) override;
    
        DynamicUploadBuffer(const DynamicUploadBuffer& _other) = delete;
        DynamicUploadBuffer(DynamicUploadBuffer&& _other) = delete;
        DynamicUploadBuffer& operator=(DynamicUploadBuffer&& _other) = delete;
        DynamicUploadBuffer& operator=(const DynamicUploadBuffer& _other) = delete;
    
        ~DynamicUploadBuffer() override = default;
    };
}