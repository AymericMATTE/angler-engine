#include "pch.h"
#include "c-array.h"

namespace angler {
    CArray::CArray(void* _data, size_t _stride, size_t _count) {
        m_count = _count;
        m_stride = _stride;
        m_data = static_cast<char*>(malloc(m_count * m_stride));
        memcpy(m_data, _data, m_count * m_stride);
    }

    void* CArray::operator[](size_t _index) const {
        if (_index >= m_count)
            return nullptr;
        
        return m_data + _index * m_stride;
    }

    size_t CArray::stride() const {
        return m_stride;
    }

    size_t CArray::count() const {
        return m_count;
    }

    void* CArray::data() const {
        return m_data;
    }
}