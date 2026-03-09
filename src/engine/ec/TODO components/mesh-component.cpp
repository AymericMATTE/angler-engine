#include "pch.h"
#include "mesh-component.h"

namespace angler {
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
    
    Mesh* MeshComponent::getMesh() const {
        return m_mesh;
    }
    Material* MeshComponent::getMaterial() const {
        return m_material;
    }
}