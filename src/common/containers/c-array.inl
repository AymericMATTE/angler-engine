#include "c-array.h"

#pragma once

namespace angler {
    template <typename T>
    CArray::CArray(void* _data, size_t _count) {
        CArray(_data, sizeof(T), _count);
    }
}