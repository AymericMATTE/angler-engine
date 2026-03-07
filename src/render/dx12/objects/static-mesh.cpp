#include "pch.h"
#include "static-mesh.h"

#include "geometry.h"

#include "../buffers/static-upload-buffer.h"

namespace angler {
    StaticMesh::StaticMesh(const Geometry& _geometry) : Mesh(),
    m_vertexBuffer(_geometry.getVertices().data(), static_cast<uint>(_geometry.getVertices().count()), static_cast<uint>(_geometry.getVertices().stride())),
    m_indexBuffer(_geometry.getIndices().data(), static_cast<uint>(_geometry.getIndices().count()), sizeof(uint32)) {
        m_vertexTypeSize = static_cast<uint>(_geometry.getVertices().stride());
        m_vertexCount = static_cast<uint>(_geometry.getVertices().count());
        
        m_indexCount = static_cast<uint32>(_geometry.getIndices().count());
        
        m_vertexBufferView.BufferLocation = m_vertexBuffer.getGPUAddress();
        m_vertexBufferView.StrideInBytes = m_vertexTypeSize;
        m_vertexBufferView.SizeInBytes = m_vertexTypeSize * m_vertexCount;
        
        m_indexBufferView.BufferLocation = m_indexBuffer.getGPUAddress();
        m_indexBufferView.Format = m_indexFormat;
        m_indexBufferView.SizeInBytes = sizeof(uint32) * m_indexCount;

        m_bounds.max = _geometry.getMax();
        m_bounds.min = _geometry.getMin();
    }

    D3D12_VERTEX_BUFFER_VIEW StaticMesh::getVertexBufferView() const {
        return m_vertexBufferView;
    }

    D3D12_INDEX_BUFFER_VIEW StaticMesh::getIndexBufferView() const {
        return m_indexBufferView;
    }
}