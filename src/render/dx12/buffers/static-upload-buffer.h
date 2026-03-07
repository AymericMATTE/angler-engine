#pragma once

// Local dependencies
#include "../graphics/graphics.h"
#include "static-upload-base.h"

namespace angler {
    class StaticUploadBuffer final : public StaticUploadBase {
    public:
        StaticUploadBuffer() = default;
        template<typename T>
        StaticUploadBuffer(const T* _data, uint _elementCount, uint _elementSize = sizeof(T));

        template<typename T>
        void init(const T* _data, uint _elementCount, uint _elementSize = sizeof(T));

        StaticUploadBuffer(const StaticUploadBuffer& _other) = delete;
        StaticUploadBuffer(StaticUploadBuffer&& _other) = delete;
        StaticUploadBuffer& operator=(const StaticUploadBuffer& _other) = delete;
        StaticUploadBuffer& operator=(StaticUploadBuffer&& _other) = delete;

       [[nodiscard]] uint getSize() const override;
    
        ~StaticUploadBuffer() override = default;
    };
}
#include "static-upload-buffer.inl"