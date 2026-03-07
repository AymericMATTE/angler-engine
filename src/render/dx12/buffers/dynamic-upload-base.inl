#pragma once
#include "dynamic-upload-base.h"

namespace angler {
    template<typename T>
    void DynamicUploadBase::copyData(const T* _data, const uint _index, const uint _size)
    {
        if (_index + _size > m_bufferSize)
            return;

        memcpy((char*)m_mappedData + _index, _data, _size);
    }
}
