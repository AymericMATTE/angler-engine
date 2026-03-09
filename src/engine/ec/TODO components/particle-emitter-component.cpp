#include "pch.h"
#include "particle-emitter-component.h"

#include <array>

#include "application.h"

namespace angler {

    void ParticleEmitterComponent::setParticlesCount(int32 _count) {
        m_properties.particlesCount = _count;
    }
    
    void ParticleEmitterComponent::setSpawnRadius(float _radius) {
        m_properties.spawnRadius = _radius;
    }
    
    void ParticleEmitterComponent::setSpeedMax(float _speedMax) {
        m_properties.speedMax = _speedMax;
    }
    
    void ParticleEmitterComponent::setSpeedMin(float _speedMin) {
        m_properties.speedMin = _speedMin;
    }
    
    void ParticleEmitterComponent::setLifetimeMax(float _lifetimeMax) {
        m_properties.lifetimeMax = _lifetimeMax;
    }
    
    void ParticleEmitterComponent::setLifetimeMin(float _lifetimeMin) {
        m_properties.lifetimeMin = _lifetimeMin;
    }
    
    void ParticleEmitterComponent::setRespawnTimeMax(float _respawnMin) {
        m_properties.respawnTimeMax = _respawnMin;
    }
    
    void ParticleEmitterComponent::setRespawnTimeMin(float _respawnMax) {
        m_properties.respawnTimeMin = _respawnMax;
    }
    
    void ParticleEmitterComponent::setScaleMax(float _scaleMin) {
        m_properties.scaleMax = _scaleMin;
    }
    
    void ParticleEmitterComponent::setScaleMin(float _scaleMax) {
        m_properties.scaleMin = _scaleMax;
    }
    void ParticleEmitterComponent::setBillBoardMode(BillBoard _mode){
        m_properties.mode = _mode;
    }

    void ParticleEmitterComponent::addParticleColor(const DirectX::XMFLOAT4& _color) {
        if (m_cursor >= MAX_COLOR) return;
        
        m_properties.particlesColor[m_cursor] = _color;
        m_cursor++;
        m_properties.colorCount = m_cursor;
    }
    
    void ParticleEmitterComponent::setMesh(Mesh* _mesh) {
        m_properties.particlesPattern = _mesh;
    }

    void ParticleEmitterComponent::setTexture(Texture* _texture) {
        m_properties.texture = _texture;
    }

    void ParticleEmitterComponent::defaultWithMesh(Mesh* _mesh) {
        m_properties.defaultWithMesh(_mesh);
    }

    void ParticleEmitterComponent::initialize() {
        delete m_emitter;
        m_emitter = new ParticlesEmitter(
            APPLICATION.getRenderer().getDefaultParticleShader(),
            m_properties
        );
    }
    
    void ParticleEmitterComponent::play() {
        m_emitter->play();
    }
    
    void ParticleEmitterComponent::pause() {
        m_emitter->pause();
    }
    
    void ParticleEmitterComponent::start() {
        m_emitter->start();
    }
    
    void ParticleEmitterComponent::stop() {
        m_emitter->stop();
    }
    
    void ParticleEmitterComponent::clear() {
        m_emitter->clear();
    }
    ParticleEmitterComponent::~ParticleEmitterComponent() {
        delete m_emitter;
        m_emitter = nullptr;
    }
}