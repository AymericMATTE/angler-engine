#pragma once

// Local dependencies
#include "static-upload-base.h"

namespace angler {
    class StaticConstantBuffer final : public StaticUploadBase {
    public:

        StaticConstantBuffer() = default;
        template<typename T>
        StaticConstantBuffer(const T* _data, uint _elementCount, uint _elementSize = sizeof(T));

        template<typename T>
        void init(const T* _data, uint _elementCount, uint _elementSize = sizeof(T));

        StaticConstantBuffer(const StaticConstantBuffer& _other) = delete;
        StaticConstantBuffer(StaticConstantBuffer&& _other) noexcept = delete;
        StaticConstantBuffer& operator=(const StaticConstantBuffer& _other) = delete;
        StaticConstantBuffer& operator=(StaticConstantBuffer&& _other) noexcept = delete;
        
        [[nodiscard]] uint getSize() const override;

        ~StaticConstantBuffer() override = default;
    };
}
#include "static-constant-buffer.inl"