#include "pch.h"
#include "dynamic-constant-buffer.h"
#include "../graphics/graphics.h"

namespace angler {
    DynamicConstantBuffer::DynamicConstantBuffer(const uint _bufferSize) {
        init(_bufferSize);
    }

    void DynamicConstantBuffer::init(const uint _bufferSize) {
        uint bufferSize = CalcConstantBufferByteSize(_bufferSize);
        resizeBuffer(bufferSize);
    }
}