#pragma once

// External dependencies
#include <common/maths/maths.h>

// Local dependencies
#include "chunk.h"
#include "config.h"
#include "lvl-chunk-component.h"

inline 
ChunkType chunkTypes[NUM_CHUNKS_TYPE] = {
    {"sewers-flat-straight-var0", {0.0f, 0.0f, -6.77386f}, {0.0f, 0.0f, 0.0f, 1.0f},
        {
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
            {{0.0f, 0.0f, -3.38693f}, {0.0f, 0.0f, -1.0f}},
            {{0.0f, 0.0f, -6.77386f}, {0.0f, 0.0f, -1.0f}}
        },
        {
            {{-1.94124f, 0.0f, -1.67108f}, angler::eulerToQuat(0.0f, 0.0f, 0.0f)},
            {{1.94124f, 0.0f, -1.67108f}, angler::eulerToQuat(0.0f, 0.0f, 0.0f)},
            {{-1.94124f, 0.0f, -4.99888f}, angler::eulerToQuat(0.0f, 0.0f, 0.0f)},
            {{1.94124f, 0.0f, -4.99888f}, angler::eulerToQuat(0.0f, 0.0f, 0.0f)}
        }
    },
    {"sewers-flat-left-var0", {-8.79341f, 0.0f, -8.46421f}, {0.0f, 0.707f, 0.0f, 0.707f},
        {
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
            {{-0.0f, 0.0f, -2.77386f}, {0.0f, 0.0f, -1.0}},
            {{-0.472758f, 0.0f, -4.97788f}, {0.0f, 0.0f, 0.0}},
            {{-1.75295f, 0.0f, -6.83323f}, {0.0f, 0.0f, 0.0}},
            {{-3.64569f, 0.0f, -8.05746f}, {0.0f, 0.0f, 0.0}},
            {{-5.86284f, 0.0f, -8.46421f}, {-1.0f, 0.0f, 0.0}},
            {{-8.79341f, 0.0f, -8.46421f}, {-1.0f, 0.0f, 0.0f}}
        },
        {
            {{-1.9916f, 0.0f, -2.05947f}, angler::eulerToQuat(0.0f, 0.0f, 0.0f)},
            {{1.48538f, 0.0f, -4.72419f}, angler::eulerToQuat(0.0f, -8.78148f, 0.0f)},
            {{-1.74832f, 0.0f, -9.0427f}, angler::eulerToQuat(0.0f, 45.0f, 0.0f)},
        }
    },
    {"sewers-flat-right-var0", {8.79341f, 0.0f, -8.46421f}, {0.0f, -0.707f, 0.0f, 0.707f},
        {
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
            {{0.0f, 0.0f, -2.77386f}, {0.0, 0.0f, -1.0}},
            {{0.472758f, 0.0f, -4.97788f}, {0.0, 0.0f, 0.0}},
            {{1.75295f, 0.0f, -6.83323f}, {0.0, 0.0f, 0.0}},
            {{3.64569f, 0.0f, -8.05746f}, {0.0, 0.0f, 0.0}},
            {{5.86284f, 0.0f, -8.46421f}, {1.0, 0.0f, 0.0}},
            {{8.79341f, 0.0f, -8.46421f}, {1.0f, 0.0f, 0.0f}}
        }, 
        {
            {{1.9916f, 0.0f, -2.05947f}, angler::eulerToQuat(0.0f, 0.0f, 0.0f)},
            {{-1.48538f, 0.0f, -4.72419f}, angler::eulerToQuat(0.0f, -8.78148f, 0.0f)},
            {{1.74832f, 0.0f, -9.0427f}, angler::eulerToQuat(0.0f, -57.2565f, 0.0f)},
        }
    },
    {"sewers-up-straight-var0", {0.0f, 1.0f, -6.77386f}, {0.0f, 0.0f, 0.0f, 1.0f},
        {
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{0.0f, 0.5f, -3.38693f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, 1.0f, -6.77386f}, {0.0f, 0.0f, -1.0f}}
        },
        {
        {{-1.94124f, 0.244124f, -1.67108f}, angler::eulerToQuat(8.46103f, 0.0f, 0.0f)},
        {{1.94124f, 0.244124f, -1.67108f}, angler::eulerToQuat(8.46103f, 0.0f, 0.0f)},
        {{-1.94124f, 0.734228f, -4.99888f}, angler::eulerToQuat(8.46103f, 0.0f, 0.0f)},
        {{1.94124f, 0.734228f, -4.99888f}, angler::eulerToQuat(8.46103f, 0.0f, 0.0f)}
        }
    },
    {"sewers-down-straight-var0", {0.0f, -1.0f, -6.77386f}, {0.0f, 0.0f, 0.0f, 1.0f},
        {
        {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{0.0f, -0.5f, -3.38693f}, {0.0f, 0.0f, 0.0f}},
        {{0.0f, -1.0f, -6.77386f}, {0.0f, 0.0f, -1.0f}}
        },
        {
        {{-1.94124f, -0.244124f, -1.67108f}, angler::eulerToQuat(-8.46103f, 0.0f, 0.0f)},
        {{1.94124f, -0.244124f, -1.67108f}, angler::eulerToQuat(-8.46103f, 0.0f, 0.0f)},
        {{-1.94124f, -0.734228f, -4.99888f}, angler::eulerToQuat(-8.46103f, 0.0f, 0.0f)},
        {{1.94124f, -0.734228f, -4.99888f}, angler::eulerToQuat(-8.46103f, 0.0f, 0.0f)}
        }
    },
};

inline angler::uint s_nextGeneration = 0;
    
angler::ECS::Entity spawnChunkAbsolute(angler::uint _type, const DirectX::XMFLOAT3& _worldPos, const DirectX::XMFLOAT4& _worldRot);

angler::ECS::Entity spawnFirstChunk();

angler::ECS::Entity spawnNextChunk(angler::uint _type, angler::ECS::Entity _prevChunk);