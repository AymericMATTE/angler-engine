#include "pch.h"
#include "mesh-component.h"
#include "application.h"

namespace angler {
    void MeshComponent::OnRender3D()
    {
        if (m_mesh == nullptr || m_material == nullptr) return;
        
        Application::get().getRenderer().draw(*m_mesh, getOwner()->m_transform, *m_material, m_uvOffset, m_uvScale);
    }

    MeshComponent::MeshComponent(Mesh* _mesh, Material* _material) {
        m_mesh = _mesh;
        m_material = _material;
    }
    void MeshComponent::setMesh(Mesh* _mesh) {
        m_mesh = _mesh;
    }
    void MeshComponent::setMaterial(Material* _material) {
        m_material = _material;
    }

    void MeshComponent::setUVOffset(const DirectX::XMFLOAT2& _offset)
    {
        m_uvOffset = _offset;
    }

    void MeshComponent::setUVScale(const DirectX::XMFLOAT2& _scale)
    {
        m_uvScale = _scale;
    }
    
    Mesh* MeshComponent::getMesh() const {
        return m_mesh;
    }
    Material* MeshComponent::getMaterial() const {
        return m_material;
    }
}