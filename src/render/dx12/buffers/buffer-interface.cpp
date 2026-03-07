#include "pch.h"
#include "buffer-interface.h"
#include "../srv-descriptor-heap.h"

#include <cassert>

namespace angler
{
    ISRVResource::~ISRVResource()
    {
        if (m_table != nullptr)
            m_table->remove(m_id);
    }

    [[nodiscard]] CD3DX12_CPU_DESCRIPTOR_HANDLE ISRVResource::getSRVCPUHandle() const
    {
        assert(m_table != nullptr);
        return m_table->getCPUHandle(m_id);
    }

    [[nodiscard]] CD3DX12_GPU_DESCRIPTOR_HANDLE ISRVResource::getSRVGPUHandle() const
    {
        assert(m_table != nullptr);
        return m_table->getGPUHandle(m_id);
    }
}