#include "pch.h"
#include "particles-emitter.h"

// Local dependencies
#include "../graphics/graphics.h"
#include "render/dx12/objects/texture.h"

// External dependencies
#include <ctime>



namespace angler {
	void EmitterProperties::defaultWithMesh(Mesh* _mesh) {
		particlesCount = 500;
		position = { 0.0f, 0.0f, 0.0f };
		spawnRadius = 0.5f;
		speedMin = 0.1f;
		speedMax = 0.3f;
		lifetimeMin = 0.3f;
		lifetimeMax = 0.5f;
		respawnTimeMin = 0.01f;
		respawnTimeMax = 0.05f;
		scaleMin = 0.01f;
		scaleMax = 0.03f;
		colorCount = 1;
		
		memset(particlesColor, 1, sizeof(particlesColor));

		particlesPattern = _mesh;
	}

	ParticlesEmitter::ParticlesEmitter() {
		m_emitterDataBuffer.init(sizeof(EmitterData));
		m_drawDataBuffer.init(sizeof(DrawData));
	}

	ParticlesEmitter::ParticlesEmitter(ComputeShader* _cs, EmitterProperties& _prop) {
		m_emitterDataBuffer.init(sizeof(EmitterData));
		m_drawDataBuffer.init(sizeof(DrawData));
		init(_cs, _prop);
	}

	void ParticlesEmitter::init(ComputeShader* _cs, EmitterProperties& _prop) {
		m_computeShader = _cs;

		m_emitterData.colorCount = _prop.colorCount;
		m_emitterData.lifetimeMin = _prop.lifetimeMin;
		m_emitterData.lifetimeMax = _prop.lifetimeMax;
		m_emitterData.particlesCount = _prop.particlesCount;
		m_emitterData.position = _prop.position;
		m_emitterData.spawnRadius = _prop.spawnRadius;
		m_emitterData.respawnTimeMin = _prop.respawnTimeMin;
		m_emitterData.respawnTimeMax = _prop.respawnTimeMax;
		m_emitterData.scaleMin = _prop.scaleMin;
		m_emitterData.scaleMax = _prop.scaleMax;
		m_emitterData.speedMin = _prop.speedMin;
		m_emitterData.speedMax = _prop.speedMax;
		m_emitterData.time = rand() % 10000;
		m_emitterData.isEmitting = 0;

		memcpy(m_emitterData.particlesColor, _prop.particlesColor, _prop.colorCount * sizeof(DirectX::XMFLOAT4));

		m_pattern = _prop.particlesPattern;
		m_drawData.billBoardMode = _prop.mode;

		if(_prop.texture != nullptr)
			m_drawData.albedoID = _prop.texture->getId();

		std::vector<ParticleData> particlesData(m_emitterData.particlesCount, ParticleData());
		m_particlesDataBuffer.init(particlesData.data(), m_emitterData.particlesCount, sizeof(ParticleData));

		m_emitterDataBuffer.copyData(&m_emitterData, 0);
		m_drawDataBuffer.copyData(&m_drawData, 0);
	}

	void ParticlesEmitter::update(float _deltatime) {
		if (m_isPaused)
			return;

		m_emitterData.deltatime = _deltatime;
		m_emitterData.time = rand() % 10000;
		m_emitterDataBuffer.copyData(&m_emitterData, 0, sizeof(EmitterData));

		Graphics::setComputeBuffer(0, ViewType::CBV, &m_emitterDataBuffer);
		Graphics::setComputeBuffer(1, ViewType::UAV, &m_particlesDataBuffer);

		Graphics::dispatch(m_computeShader, (m_emitterData.particlesCount + 255) / 256, 1, 1);
	}

	void ParticlesEmitter::start() {
		m_emitterData.isEmitting = 1;
		m_emitterDataBuffer.copyData(&m_emitterData, 0, sizeof(EmitterData));
	}

	void ParticlesEmitter::play() {
		m_isPaused = false;
	}

	void ParticlesEmitter::pause() {
		m_isPaused = true;
	}

	void ParticlesEmitter::stop() {
		m_emitterData.isEmitting = 0;
		m_emitterDataBuffer.copyData(&m_emitterData, 0, sizeof(EmitterData));
	}

	void ParticlesEmitter::clear() {
		assert(Graphics::isDrawingStarted() == false);

		m_isPaused = true;
		m_emitterData.isEmitting = 0;
		m_emitterDataBuffer.copyData(&m_emitterData, 0, sizeof(EmitterData));

		std::vector<ParticleData> particlesData(m_emitterData.particlesCount, ParticleData());
		m_particlesDataBuffer.init(particlesData.data(), m_emitterData.particlesCount, sizeof(ParticleData));
	}

	void ParticlesEmitter::setPosition(DirectX::XMFLOAT3 _pos) {
		m_emitterData.position = _pos;
	}

	uint ParticlesEmitter::getParticlesCount() const {
		return m_emitterData.particlesCount;
	}

	DynamicConstantBuffer const* ParticlesEmitter::getEmitterData() const {
		return &m_emitterDataBuffer;
	}

	StaticConstantBuffer const* ParticlesEmitter::getParticlesData() const {
		return &m_particlesDataBuffer;
	}

	Mesh const* ParticlesEmitter::getPattern() const {
		return m_pattern;
	}

	DynamicConstantBuffer const* ParticlesEmitter::getDrawData() const {
		return &m_drawDataBuffer;
	}
}