#include "geometry.h"

#pragma once

namespace angler {
    template <typename VertexType>
    Geometry::Geometry(const std::vector<VertexType>& _vertices, const std::vector<uint32>& _indices, Dimension _dimension) :
        m_vertices(_vertices.data(), _vertices.size()),
        m_indices(_indices.data(), _indices.size()) {

        switch (_dimension)
        {
        case _2D:
            GetMinMax2D();
            break;
        case _3D:
            GetMinMax3D();
            break;
        default:
            assert(false && "Not valid dimension");
            break;
        }
    }

    template <typename VertexType>
    Geometry::Geometry(VertexType* _vertices, size_t _vertexCount, uint32* _indices, size_t _indexCount, Dimension  _dimension) :
        m_vertices(_vertices, sizeof(VertexType), _vertexCount),
        m_indices(_indices, sizeof(uint32), _indexCount) {

        switch (_dimension)
        {
        case _2D:
            GetMinMax2D();
            break;
        case _3D:
            GetMinMax3D();
            break;
        default:
            assert(false && "Not valid dimension");
            break;
        }
    }

    inline Geometry::Geometry(void* _vertices, size_t _vertexStride, size_t _vertexCount, uint32* _indices, size_t _indexCount, Dimension  _dimension) :
    m_vertices(_vertices, _vertexStride, _vertexCount),
    m_indices(_indices, sizeof(uint32), _indexCount) {

        switch (_dimension)
        {
        case _2D:
            GetMinMax2D();
            break;
        case _3D:
            GetMinMax3D();
            break;
        default:
            assert(false && "Not valid dimension");
            break;
        }
    }
}