#pragma once

#include "render/dx12/particles-emitter/particles-emitter.h"
#include "engine/ec/component-base.h"

namespace angler {
    struct ParticleEmitterComponent: public ComponentBase<ParticleEmitterComponent> {
        void setParticlesCount(int32 _count);
        void setSpawnRadius(float _radius);
        void setSpeedMax(float _speedMax);
        void setSpeedMin(float _speedMin);
        void setLifetimeMax(float _lifetimeMax);
        void setLifetimeMin(float _lifetimeMin);
        void setRespawnTimeMax(float _respawnMin);
        void setRespawnTimeMin(float _respawnMax);
        void setScaleMax(float _scaleMin);
        void setScaleMin(float _scaleMax);
        void setBillBoardMode(BillBoard _mode);
        
        void addParticleColor(const DirectX::XMFLOAT4& _color);
        void setMesh(Mesh* _mesh);
        void setTexture(Texture* _texture);
        
        void defaultWithMesh(Mesh* _mesh);
        
        /// WARNING : Call AFTER doing all the emitter configuration, overwise won't work
        void initialize();
        
        void play();
        void pause();

        void start();
        void stop();

        void clear();

        void OnRender3D();

        ~ParticleEmitterComponent();
    
    private:
        ParticlesEmitter* m_emitter = nullptr;
        EmitterProperties m_properties = {};
        int m_cursor = 0;
        
        friend class Application;
    };
}