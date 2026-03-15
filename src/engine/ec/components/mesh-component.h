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
        
        Mesh* getMesh() const;
        Material* getMaterial() const;

        void OnRender3D();
    
    private:
        Mesh* m_mesh;
        Material* m_material;
        friend class Application;
    };
}