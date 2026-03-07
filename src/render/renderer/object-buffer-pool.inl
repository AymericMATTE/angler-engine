#include "object-buffer-pool.h"

#pragma once

namespace angler {
    template <uint MaxObjects>
    void ObjectBufferPool<MaxObjects>::init(size_t objectSize) {
        buffers.reserve(MaxObjects);
        for (uint i = 0; i < MaxObjects; ++i) {
            auto* buf = new DynamicConstantBuffer();
            buf->init(static_cast<uint>(objectSize));
            buffers.push_back(buf);
        }
    }
    
    template <uint MaxObjects>
    void ObjectBufferPool<MaxObjects>::cleanup() {
        for (auto* buf : buffers) delete buf;
        buffers.clear();
        cursor = 0;
    }
    
    template <uint MaxObjects>
    void ObjectBufferPool<MaxObjects>::reset() {
        cursor = 0;
    }
}