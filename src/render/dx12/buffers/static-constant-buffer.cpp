#include "pch.h"
#include "static-constant-buffer.h"

namespace angler {
   uint StaticConstantBuffer::getSize() const {
        return CalcConstantBufferByteSize(m_elementCount * m_elementSize);
    }
}