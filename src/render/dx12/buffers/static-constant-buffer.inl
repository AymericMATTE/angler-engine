#pragma once
#include "static-constant-buffer.h"
#include "../utils/error-helper.h"

namespace angler {
    template<typename T>
    StaticConstantBuffer::StaticConstantBuffer(const T* _data, const uint _elementCount, const uint _elementSize) {
        init(_data, _elementCount, _elementSize);
    }

    template<typename T>
    void StaticConstantBuffer::init(const T* _data, const uint _elementCount, const uint _elementSize)
    {
        DX12_OBJ_RELEASE(m_resourceBuffer);

        m_elementCount = _elementCount;
        m_elementSize = _elementSize;
        const uint bufferSize = CalcConstantBufferByteSize(_elementSize * _elementCount);

        uploadBuffers(_data, bufferSize);
    }
}