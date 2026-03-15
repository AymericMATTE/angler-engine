#pragma once

#include "render/dx12/objects/mesh.h"
#include "render/renderer/material.h"
#include "engine/ec/component-base.h"

namespace angler {
    class MeshComponent: public ComponentBase<MeshComponent>{
    public:
        MeshComponent() = default;
        MeshComponent(Mesh* _mesh, Material* _material);
        
        void setMesh(Mesh* _mesh);
        void setMaterial(Material* _material);
        void setUVOffset(const DirectX::XMFLOAT2& _offset);
        void setUVScale(const DirectX::XMFLOAT2& _scale);
        
        Mesh* getMesh() const;
        Material* getMaterial() const;

        void OnRender3D();
    
    private:
        Mesh* m_mesh;
        Material* m_material;

        DirectX::XMFLOAT2 m_uvOffset = { 0.0f, 0.0f };
        DirectX::XMFLOAT2 m_uvScale = { 1.0f, 1.0f };
        friend class Application;
    };
}