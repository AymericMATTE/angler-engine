#pragma once

// Local dependencies
#include "common/types.h"
#include "common/containers/c-array.h"

// External dependencies
#include <vector>
#include <DirectXMath.h>

namespace angler {
    class Geometry final {
    public:
        enum Dimension
        {
            _2D,
            _3D
        };

        template<typename VertexType>
        Geometry(const std::vector<VertexType>& _vertices, const std::vector<uint32>& _indices, Dimension _dimension);
        template<typename VertexType>
        Geometry(VertexType* _vertices, size_t _vertexCount, uint32* _indices, size_t _indexCount, Dimension  _dimension);
        Geometry(void* _vertices, size_t _vertexStride, size_t _vertexCount, uint32* _indices, size_t _indexCount, Dimension  _dimension);
        
        const CArray& getVertices() const;
        const CArray& getIndices() const;

        DirectX::XMFLOAT3 getMax() const;
        DirectX::XMFLOAT3 getMin() const;

    private:
        void GetMinMax2D();
        void GetMinMax3D();

        CArray m_vertices;
        CArray m_indices;

        Dimension m_dimension;
        DirectX::XMFLOAT3 m_max;
        DirectX::XMFLOAT3 m_min;
    };
}
#include "geometry.inl"