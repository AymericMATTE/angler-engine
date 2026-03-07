#pragma once
#include "dynamic-upload-base.h"

namespace angler {
    class DynamicConstantBuffer final : public DynamicUploadBase {
    public:
        DynamicConstantBuffer() = default;
        explicit DynamicConstantBuffer(uint _bufferSize);

        void init(uint _bufferSize) override;

        DynamicConstantBuffer(const DynamicConstantBuffer& _other) = delete;
        DynamicConstantBuffer(DynamicConstantBuffer&& _other) = delete;
        DynamicConstantBuffer& operator=(DynamicConstantBuffer&& _other) = delete;
        DynamicConstantBuffer& operator=(const DynamicConstantBuffer& _other) = delete;
        
        ~DynamicConstantBuffer() override = default;
    };
}