#include "pch.h"
#include "dynamic-upload-buffer.h"
#include "../graphics/graphics.h"

namespace angler {
    DynamicUploadBuffer::DynamicUploadBuffer(const uint _bufferSize) {
        init(_bufferSize);
    }

    void DynamicUploadBuffer::init(const uint _bufferSize) {
        resizeBuffer(_bufferSize);
    }
}