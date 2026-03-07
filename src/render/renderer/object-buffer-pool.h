#pragma once


#include "../dx12/buffers/dynamic-constant-buffer.h"

#include <vector>

namespace angler {
    template<uint MaxObjects>
    struct ObjectBufferPool {
        std::vector<DynamicConstantBuffer*> buffers;
        uint cursor = 0;

        void init(size_t objectSize);

        void cleanup();

        DynamicConstantBuffer* next() {
            return buffers[cursor++];
        }

        void reset();
    };
}
#include "object-buffer-pool.inl"