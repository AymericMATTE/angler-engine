#include "pch.h"
#include "lvl-generator.h"

// Local dependencies
#include "game-resources.h"

// External dependencies
#include <ecs/ecs.h>
#include <application.h>
#include <resource-manager.h>
#include <ecs/components/mesh-component.h>
#include <ecs/components/transform-component.h>
#include <game/crate-component.h>

#include "enemy.h"
#include "ecs/components/collider-component.h"

using namespace angler;

ECS::Entity createCrate(TransformComponent& _enemyTransform) {
    ECS& ecs = APPLICATION.getECS();
    ECS::Entity crate = ecs.createEntity();
    TransformComponent& transform = ecs.addComponent<TransformComponent>(crate);
    CrateComponent& crateComp = ecs.addComponent<CrateComponent>(crate);
    ColliderComponent& collider = ecs.addComponent<ColliderComponent>(crate);
    MeshComponent& mesh = ecs.addComponent<MeshComponent>(crate);

    DirectX::XMFLOAT3 enemyPosition = _enemyTransform.getPosition();
    DirectX::XMFLOAT3 enemyForward = _enemyTransform.getForward();
    float forwardFactor = 1.f;
    transform.setPosition({ enemyPosition.x + enemyForward.x * forwardFactor,
        enemyPosition.y + enemyForward.y * forwardFactor,
        enemyPosition.z + enemyForward.z * forwardFactor });

    mesh.setMaterial(g_res.crateMaterial);
    mesh.setMesh(g_res.crateMesh);

    collider.SetBox();

    return crate;
}

ECS::Entity spawnChunkAbsolute(uint _type, const DirectX::XMFLOAT3& _worldPos, const DirectX::XMFLOAT4& _worldRot) {
    ECS& ecs = APPLICATION.getECS();

    const ChunkType& type = chunkTypes[_type];
    const ChunkResource& res = g_res.chunkResources[_type];

    // Load resources
    StaticMesh* chunkModel = res.bodyMesh;
    StaticMesh* waterModel = res.waterMesh;

    Material* chunkMaterial = res.bodyMaterial;
    Material* waterMaterial = g_res.waterMaterial;

    // Create chunk entity
    ECS::Entity chunkEntity = ecs.createEntity();
    TransformComponent& chunkTransform = ecs.addComponent<TransformComponent>(chunkEntity);
    chunkTransform.setPosition(_worldPos);
    chunkTransform.setRotation(_worldRot);
    MeshComponent& chunkMesh = ecs.addComponent<MeshComponent>(chunkEntity);
    chunkMesh.setMesh(chunkModel);
    chunkMesh.setMaterial(chunkMaterial);

    // Create water entity
    ECS::Entity waterEntity = ecs.createEntity();
    TransformComponent& waterTransform = ecs.addComponent<TransformComponent>(waterEntity);
    waterTransform.setPosition(_worldPos);
    waterTransform.setRotation(_worldRot);
    MeshComponent& waterMesh = ecs.addComponent<MeshComponent>(waterEntity);
    waterMesh.setMesh(waterModel);
    waterMesh.setMaterial(waterMaterial);

    // Create exit point
    ECS::Entity exitEntity = ecs.createEntity();
    TransformComponent& exitTransform = ecs.addComponent<TransformComponent>(exitEntity);
    exitTransform.setParent(&chunkTransform);
    exitTransform.setLocalPosition(type.exitPos);
    exitTransform.setLocalRotation(type.exitRot);

    // Setup chunk data
    ChunkComponent& chunkComp = ecs.addComponent<ChunkComponent>(chunkEntity);
    chunkComp.exitEntity = exitEntity;
    chunkComp.generation = s_nextGeneration++;
    chunkComp.typeIndex = static_cast<byte>(_type);

    chunkComp.worldPos = _worldPos;
    chunkComp.worldRot = _worldRot;
    chunkComp.waterEntity = waterEntity;
    
    // Create associated enemies
    for (auto& spawner : type.spawners) {
        int random = rand() % 100;
        if (random <= ENEMY_SPAWN_CHANCE) {
            EnemyType randType = static_cast<EnemyType>(rand() % 2);
            ECS::Entity enemy = spawnEnemy(randType, &chunkComp, &chunkTransform, spawner.pos, spawner.rot);

            if (randType == EnemyType::Hidden) chunkComp.crates.push_back(createCrate(ecs.getComponent<TransformComponent>(enemy)));

            chunkComp.enemies.push_back(enemy);
        }
    }

    return chunkEntity;
}

ECS::Entity spawnFirstChunk() {
    return spawnChunkAbsolute(0, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
}

ECS::Entity spawnNextChunk(uint _type, ECS::Entity _prevChunk) {
    ECS& ecs = APPLICATION.getECS();

    ChunkComponent& prevComp = ecs.getComponent<ChunkComponent>(_prevChunk);
    TransformComponent& prevExit = ecs.getComponent<TransformComponent>(prevComp.exitEntity);

    // Get exit point
    DirectX::XMFLOAT3 exitPos = prevExit.getPosition();
    DirectX::XMFLOAT4 exitRot = prevExit.getRotation();

    ECS::Entity newChunk = spawnChunkAbsolute(_type, exitPos, exitRot);

    ChunkComponent& newComp = ecs.getComponent<ChunkComponent>(newChunk);
    newComp.previous = _prevChunk;
    prevComp.next = newChunk;

    return newChunk;
}