#include "pch.h"
#include "static-upload-buffer.h"

namespace angler {
    uint StaticUploadBuffer::getSize() const {
        return m_elementCount * m_elementSize;
    }
}