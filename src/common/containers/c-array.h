#pragma once

namespace angler {
    class CArray {
    public:
        CArray() = default;
        template <typename T>
        CArray(void* _data, size_t _count);
        CArray(void* _data, size_t _stride, size_t _count);
        
        void* operator[](size_t _index) const;
        size_t stride() const;
        size_t count() const;
        void* data() const;
        
    private:
        char* m_data    = nullptr;
        size_t m_count  = 0;
        size_t m_stride = 0;
    };
}
#include "c-array.inl"