#pragma once

// Local dependencies
#include "mesh.h"
#include "../buffers/static-upload-buffer.h"

// Forward declarations
namespace angler {
    class Geometry;
}

namespace angler {
    class StaticMesh final : public Mesh {
    public:
        explicit StaticMesh(const Geometry& _geometry);
        
        [[nodiscard]] D3D12_VERTEX_BUFFER_VIEW getVertexBufferView() const override;
        [[nodiscard]] D3D12_INDEX_BUFFER_VIEW getIndexBufferView() const override;
        
    private:
        StaticUploadBuffer m_vertexBuffer;
        StaticUploadBuffer m_indexBuffer;
        
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
    };
}