#include "pch.h"
#include "mesh.h"

namespace angler {
    int Mesh::getVertexCount() const {
        return m_vertexCount;
    }

    uint32 Mesh::getIndexCount() const {
        return m_indexCount;
    }

    AABB Mesh::getBoundingBox() const
    {
        return m_bounds;
    }
}