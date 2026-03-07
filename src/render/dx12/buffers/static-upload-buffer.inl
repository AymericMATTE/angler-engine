#pragma once
#include "static-upload-buffer.h"
#include "../utils/error-helper.h"

namespace angler {
    template<typename T>
    StaticUploadBuffer::StaticUploadBuffer(const T* _data, const uint _elementCount, const uint _elementSize) {
        init(_data, _elementCount, _elementSize);
    }

    template<typename T>
    inline void StaticUploadBuffer::init(const T* _data, uint _elementCount, uint _elementSize)
    {
        DX12_OBJ_RELEASE(m_resourceBuffer);

        m_elementSize = _elementSize;
        m_elementCount = _elementCount;
        uploadBuffers(_data, m_elementSize * m_elementCount);
    }
}