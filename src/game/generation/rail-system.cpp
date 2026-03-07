#include "pch.h"
#include "rail-system.h"

#include "game-state.h"
#include "lvl-chunk-component.h"
#include "lvl-generator.h"

using namespace angler;

bool isVecZero(const DirectX::XMFLOAT3& v, float epsilon = 0.0001f) {
    return fabs(v.x) < epsilon && fabs(v.y) < epsilon && fabs(v.z) < epsilon;
}

DirectX::XMFLOAT3 normalize(const DirectX::XMFLOAT3& _vector) {
    float len = sqrtf(_vector.x * _vector.x + _vector.y * _vector.y + _vector.z * _vector.z);
    if (len < 0.0001f) return { 0.0f, 0.0f, -1.0f };
    return { _vector.x / len, _vector.y / len, _vector.z / len };
}

DirectX::XMFLOAT3 localToWorld(TransformComponent& chunkTransform, const DirectX::XMFLOAT3& localPos) {
    DirectX::XMFLOAT4X4 worldMat = chunkTransform.getWorldMatrix();
    DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&worldMat);

    DirectX::XMVECTOR localVec = DirectX::XMLoadFloat3(&localPos);
    DirectX::XMVECTOR worldVec = DirectX::XMVector3Transform(localVec, mat);

    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, worldVec);
    return result;
}

DirectX::XMFLOAT3 localDirToWorld(TransformComponent& chunkTransform, const DirectX::XMFLOAT3& localDir) {
    DirectX::XMFLOAT4 rot = chunkTransform.getRotation();
    DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rot));

    DirectX::XMVECTOR localVec = DirectX::XMLoadFloat3(&localDir);
    DirectX::XMVECTOR worldVec = DirectX::XMVector3TransformNormal(localVec, rotMat);

    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, worldVec);
    return normalize(result);
}

void railSystemFunction(ECS::Entity _entity, RailFollower& _follower, TransformComponent& _transform) {
    if (!_follower.active)
        return;
    
    ECS& ecs = APPLICATION.getECS();
    float delta = APPLICATION.deltaTime();
    
    // Init first chunk
    if (_follower.currentChunk == ECS::invalidEntity) {
        ecs.foreach<ChunkComponent>([&](ECS::Entity ent, ChunkComponent& chunk) {
            if (_follower.currentChunk == ECS::invalidEntity && chunk.generation == 0) {
                _follower.currentChunk = ent;
            }
        });
        if (_follower.currentChunk == ECS::invalidEntity) return;
    }
    
    // Get current chunk
    ChunkComponent& chunkComp = ecs.getComponent<ChunkComponent>(_follower.currentChunk);
    TransformComponent& chunkTransform = ecs.getComponent<TransformComponent>(_follower.currentChunk);
    const ChunkType& chunkType = chunkTypes[chunkComp.typeIndex];
    const auto& checkpoints = chunkType.checkpoints;
    
    if (checkpoints.size() < 2) return;
    
    // Calculate path length
    float totalLength = 0.0f;
    for (size_t i = 1; i < checkpoints.size(); ++i) {
        float dx = checkpoints[i].pos.x - checkpoints[i-1].pos.x;
        float dy = checkpoints[i].pos.y - checkpoints[i-1].pos.y;
        float dz = checkpoints[i].pos.z - checkpoints[i-1].pos.z;
        totalLength += sqrtf(dx*dx + dy*dy + dz*dz);
    }
    
    // Move
    _follower.t += (_follower.speed * delta) / (totalLength > 0.001f ? totalLength : 1.0f);
    
    // Transition to next chunk
    if (_follower.t >= 1.0f) {
        float excess = _follower.t - 1.0f;
        
        if (chunkComp.next != ECS::invalidEntity) {
            if (ecs.hasComponent<ChunkComponent>(chunkComp.next)) {
                _follower.currentChunk = chunkComp.next;
                _follower.t = excess;
            } else {
                _follower.t = 1.0f;
                _follower.active = false;
            }
        } else {
            _follower.t = 1.0f;
        }
    }
    
    // Get current chunk again in case it changed
    ChunkComponent& currentChunk = ecs.getComponent<ChunkComponent>(_follower.currentChunk);
    TransformComponent& currentChunkTransform = ecs.getComponent<TransformComponent>(_follower.currentChunk);
    const ChunkType& currentType = chunkTypes[currentChunk.typeIndex];
    const auto& points = currentType.checkpoints;
    g_state.currentChunk = currentChunk.generation;
    
    // Recalculate length
    float currTotalLength = 0.0f;
    for (size_t i = 1; i < points.size(); ++i) {
        float dx = points[i].pos.x - points[i-1].pos.x;
        float dy = points[i].pos.y - points[i-1].pos.y;
        float dz = points[i].pos.z - points[i-1].pos.z;
        currTotalLength += sqrtf(dx*dx + dy*dy + dz*dz);
    }
    
    // Sample path
    float targetDist = _follower.t * currTotalLength;
    float distAccum = 0.0f;
    uint segmentIndex = 0;
    float segT = 0.0f;
    
    for (size_t i = 1; i < points.size(); ++i) {
        float dx = points[i].pos.x - points[i-1].pos.x;
        float dy = points[i].pos.y - points[i-1].pos.y;
        float dz = points[i].pos.z - points[i-1].pos.z;
        float segLen = sqrtf(dx*dx + dy*dy + dz*dz);
        
        if (distAccum + segLen >= targetDist || i == points.size() - 1) {
            segmentIndex = static_cast<uint>(i - 1);
            segT = segLen > 0.001f ? (targetDist - distAccum) / segLen : 0.0f;
            break;
        }
        distAccum += segLen;
    }
    
    // Interpolate position
    const auto& a = points[segmentIndex];
    const auto& b = points[segmentIndex + 1];
    
    DirectX::XMFLOAT3 localPos;
    localPos.x = a.pos.x + (b.pos.x - a.pos.x) * segT + _follower.offsetPosition.x;
    localPos.y = a.pos.y + (b.pos.y - a.pos.y) * segT + _follower.offsetPosition.y;
    localPos.z = a.pos.z + (b.pos.z - a.pos.z) * segT + _follower.offsetPosition.z;
    
    // Calculate look direction
    DirectX::XMFLOAT3 localLook;
    bool aHasLook = !isVecZero(a.lookAt);
    bool bHasLook = !isVecZero(b.lookAt);
    
    DirectX::XMFLOAT3 lookA, lookB;
    
    if (aHasLook) {
        lookA = a.lookAt;
    } else {
        lookA = normalize({
            b.pos.x - a.pos.x,
            b.pos.y - a.pos.y,
            b.pos.z - a.pos.z
        });
    }
    
    if (bHasLook) {
        lookB = b.lookAt;
    } else {
        if (segmentIndex + 2 < points.size()) {
            const auto& point = points[segmentIndex + 2];
            lookB = normalize({
                point.pos.x - b.pos.x,
                point.pos.y - b.pos.y,
                point.pos.z - b.pos.z
            });
        } else {
            lookB = normalize({
                b.pos.x - a.pos.x,
                b.pos.y - a.pos.y,
                b.pos.z - a.pos.z
            });
        }
    }
    
    // Interpolate look direction
    localLook.x = lookA.x + (lookB.x - lookA.x) * segT;
    localLook.y = lookA.y + (lookB.y - lookA.y) * segT;
    localLook.z = lookA.z + (lookB.z - lookA.z) * segT;
    localLook = normalize(localLook);
    
    // Transform to WORLD space
    DirectX::XMFLOAT3 worldPos = localToWorld(currentChunkTransform, localPos);
    DirectX::XMFLOAT3 worldLook = localDirToWorld(currentChunkTransform, localLook);

    float bobbingY = (std::sin(APPLICATION.time() * 4.0f) * 0.075f) - 0.02f;
    worldPos.y += bobbingY;
    
    #define PI 3.14159f
    float bobbingZ = (std::sin(APPLICATION.time() * 4.0f + PI) * BOBBING_INTENSITY);
    float bobbingX = (std::cos(APPLICATION.time() * 4.0f + PI/1.2f) * BOBBING_INTENSITY);

    _transform.setPosition(worldPos);
    _transform.lookAt({
        worldPos.x + worldLook.x, 
        worldPos.y + worldLook.y, 
        worldPos.z + worldLook.z
    });
    
    _transform.localRotate({bobbingX, 0.0f, bobbingZ});
}

void chunkCleanupSystemFunction(ECS::Entity, RailFollower& _follower) {
    if (_follower.currentChunk == ECS::invalidEntity) return;

    ECS& ecs = APPLICATION.getECS();

    if (!ecs.hasComponent<ChunkComponent>(_follower.currentChunk)) {
        _follower.currentChunk = ECS::invalidEntity;
        return;
    }

    uint currentGen = ecs.getComponent<ChunkComponent>(_follower.currentChunk).generation;

    ECS::Entity headChunk = ECS::invalidEntity;
    uint maxGen = 0;
    ecs.foreach<ChunkComponent>([&](ECS::Entity ent, ChunkComponent& chunk) {
        if (chunk.generation > maxGen) {
            maxGen = chunk.generation;
            headChunk = ent;
        }
    });

    std::vector<ECS::Entity> toDelete;
    ecs.foreach<ChunkComponent>([&](ECS::Entity ent, ChunkComponent& chunk) {
        if (currentGen > chunk.generation + 1 && !chunk.markedForDeletion) {
            toDelete.push_back(ent);
            chunk.markedForDeletion = true;
        }
    });
    
    int chunksNeeded = CHUNKS_AHEAD - static_cast<int>(maxGen - currentGen);

    ECS::Entity spawnFrom = headChunk;
    for (int i = 0; i < chunksNeeded && spawnFrom != ECS::invalidEntity; ++i) {
        int randomType = rand() % std::size(chunkTypes);
        spawnFrom = spawnNextChunk(randomType, spawnFrom);
    }

    for (ECS::Entity ent : toDelete) {
        if (!ecs.hasComponent<ChunkComponent>(ent)) continue;

        ChunkComponent& chunk = ecs.getComponent<ChunkComponent>(ent);

        if (chunk.next != ECS::invalidEntity && ecs.hasComponent<ChunkComponent>(chunk.next)) {
            ChunkComponent& nextChunk = ecs.getComponent<ChunkComponent>(chunk.next);
            nextChunk.previous = chunk.previous;
        }
        if (chunk.previous != ECS::invalidEntity && ecs.hasComponent<ChunkComponent>(chunk.previous)) {
            ChunkComponent& prevChunk = ecs.getComponent<ChunkComponent>(chunk.previous);
            prevChunk.next = chunk.next;
        }

        for (auto enemy : chunk.enemies) {
            if (enemy == ECS::invalidEntity) continue;
            ecs.destroyEntity(enemy);
        }
        for (auto crate : chunk.crates) {
            if (crate == ECS::invalidEntity) continue;
            ecs.destroyEntity(crate);
        }
        if (chunk.exitEntity != ECS::invalidEntity) {
            ecs.destroyEntity(chunk.exitEntity);
        }
        if (chunk.waterEntity != ECS::invalidEntity) {
            ecs.destroyEntity(chunk.waterEntity);
        }
        ecs.destroyEntity(ent);
    }
}