#pragma once

#define MAX_COLOR 10

// Local dependencies
#include "../utils/d3dx12.h"
#include "common/types.h"
#include "../buffers.h"

// External dependencies
#include <DirectXMath.h>

namespace angler {
	class Material;
}

namespace angler {

	enum BillBoard : uint16
	{
		None = 0,
		Spheric = 1,
		Cylindric = 2
	};

	struct EmitterProperties
	{
		int32 particlesCount = 0;

		DirectX::XMFLOAT3 position = {0.0f, 0.0f, 0.0f};

		float spawnRadius = 0.0f;
		float speedMax = 0.0f;
		float speedMin = 0.0f;
		float lifetimeMax = 0.0f;
		float lifetimeMin = 0.0f;
		float respawnTimeMax = 0.0f;
		float respawnTimeMin = 0.0f;
		float scaleMax = 0.0f;
		float scaleMin = 0.0f;

		uint32 colorCount = 0;
		DirectX::XMFLOAT4 particlesColor[MAX_COLOR]{};
		Mesh* particlesPattern = nullptr;
		Texture* texture = nullptr;
		BillBoard mode = None;
		
		void defaultWithMesh(Mesh* _mesh);
	};

	class ParticlesEmitter
	{
	public :
		ParticlesEmitter();
		ParticlesEmitter(ComputeShader* _cs, EmitterProperties& _prop);

		ParticlesEmitter(const ParticlesEmitter& _other) = delete;
		ParticlesEmitter(ParticlesEmitter&& _other) = delete;
		ParticlesEmitter& operator=(const ParticlesEmitter& _other) = delete;
		ParticlesEmitter& operator=(ParticlesEmitter&& _other) = delete;

		void init(ComputeShader* _cs, EmitterProperties& _prop);
		void update(float _deltatime);

		void play();
		void pause();

		void start();
		void stop();

		void clear();

		void setPosition(DirectX::XMFLOAT3 _pos);

		[[nodiscard]] uint getParticlesCount() const;
		[[nodiscard]] DynamicConstantBuffer const* getEmitterData() const;
		[[nodiscard]] StaticConstantBuffer const* getParticlesData() const;
		[[nodiscard]] Mesh const* getPattern() const;
		[[nodiscard]] DynamicConstantBuffer const* getDrawData() const;

		~ParticlesEmitter() = default;
	private :
		struct ParticleData
		{
			DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };
			float velocity = 0.0f;

			DirectX::XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
			float scale = 0.0f;

			DirectX::XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 0.0f };

			float lifetime = 0.0f;
			float respawnTime = 0.0f;
			uint32 isCreated = 0;
			uint32 isAlive = 0;
		};

		struct EmitterData
		{
			uint32 particlesCount;
			uint32 time;
			float deltatime;
			float spawnRadius;

			DirectX::XMFLOAT3 position;
			float speedMax;

			float speedMin;
			float scaleMax;
			float scaleMin;
			float lifetimeMax;

			float lifetimeMin;
			float respawnTimeMax;
			float respawnTimeMin;
			uint32 colorCount;

			DirectX::XMFLOAT4 particlesColor[MAX_COLOR];

			uint32 isEmitting;
		};

		struct DrawData
		{
			int32 albedoID = -1;
			uint32 billBoardMode;
		};

		EmitterData m_emitterData = {};

		DrawData m_drawData = {};

		bool m_isPaused = false;

		DynamicConstantBuffer m_emitterDataBuffer = {};

		StaticConstantBuffer m_particlesDataBuffer = {};

		DynamicConstantBuffer m_drawDataBuffer = {};

		Mesh* m_pattern = nullptr;

		ComputeShader* m_computeShader = nullptr;
	};
}