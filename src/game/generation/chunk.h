#pragma once

// External dependencies
#include <vector>
#include <DirectXMath.h>
#include <common/types.h>

struct Checkpoint {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 lookAt;
};

struct Spawnpoint {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 rot;
};

struct ChunkType {
    angler::cstr resourceName = ""; // Name, without any prefix/suffix & extension, of the resource to load
    DirectX::XMFLOAT3 exitPos = {}; // Position at wich the next chunk will be spawned
    DirectX::XMFLOAT4 exitRot = {}; // Rotation with wich the next chunk will be spawned
    std::vector<Checkpoint> checkpoints = {}; // Waypoints used to guide the rail follower object
    std::vector<Spawnpoint> spawners = {}; // Points where enemies can be spawned in the chunk
};