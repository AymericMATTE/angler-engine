#pragma once

// Local dependencies
#include "common/physics.h"
#include "common/types.h"

// External
#include <d3d12.h>

namespace angler {
    class Mesh {
    public:
        Mesh() = default;
        Mesh(const Mesh& _other) = delete;
        Mesh(Mesh&& _other) noexcept = delete;
        Mesh& operator=(const Mesh& _other) = delete;
        Mesh& operator=(Mesh&& _other) noexcept = delete;
        
        virtual ~Mesh() = default;
        
        int getVertexCount() const;
        uint32 getIndexCount() const;
        
        virtual D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const = 0;
        virtual D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const = 0;

        AABB getBoundingBox() const;

    protected:

        uint m_vertexTypeSize = 0;
        uint m_vertexCount = 0;
        uint32 m_indexCount = 0;

        AABB m_bounds;
        
        DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R32_UINT;
    };
}