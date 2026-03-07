#include "pch.h"
#include "geometry.h"

namespace angler {
    const CArray& Geometry::getVertices() const {
        return m_vertices;
    }

    const CArray& Geometry::getIndices() const {
        return  m_indices;
    }

    DirectX::XMFLOAT3 Geometry::getMax() const {
        return m_max;
    }

    DirectX::XMFLOAT3 Geometry::getMin() const {
        return m_min;
    }

    void Geometry::GetMinMax2D()
    {
        DirectX::XMVECTOR vMin = DirectX::XMVectorSet(+FLT_MAX, +FLT_MAX, +FLT_MAX, +FLT_MAX);
        DirectX::XMVECTOR vMax = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

        char* pointer = static_cast<char*>(m_vertices.data());

        for (uint i = 0; i < m_vertices.count(); ++i)
        {
            DirectX::XMVECTOR v = DirectX::XMLoadFloat2(reinterpret_cast<const DirectX::XMFLOAT2*>(pointer));

            vMin = DirectX::XMVectorMin(vMin, v);
            vMax = DirectX::XMVectorMax(vMax, v);

            pointer += m_vertices.stride();
        }

        XMStoreFloat3(&m_min, vMin);
        XMStoreFloat3(&m_max, vMax);
        m_min.z = 0;
        m_max.z = 0;
    }

    void Geometry::GetMinMax3D()
    {
        DirectX::XMVECTOR vMin = DirectX::XMVectorSet(+FLT_MAX, +FLT_MAX, +FLT_MAX, +FLT_MAX);
        DirectX::XMVECTOR vMax = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);

        char* pointer = static_cast<char*>(m_vertices.data());

        for (uint i = 0; i < m_vertices.count(); ++i)
        {
            DirectX::XMVECTOR v = DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(pointer));

            vMin = DirectX::XMVectorMin(vMin, v);
            vMax = DirectX::XMVectorMax(vMax, v);

            pointer += m_vertices.stride();
        }

        XMStoreFloat3(&m_min, vMin);
        XMStoreFloat3(&m_max, vMax);
    }
}