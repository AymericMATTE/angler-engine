#define MAX_COLOR 10
#define PI 3.14159265

struct ParticleData
{
    float3 direction;
    float velocity;

    float3 position;
    float scale;

    float4 color;

    float lifetime;
    float respawnTime;
    uint isCreated;
    uint isAlive;
};

uint PCG_Hash(uint state)
{
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float Random01(inout uint seed)
{
    seed = seed * 747796405u + 2891336453u; // LCG step
    uint rnd = PCG_Hash(seed);
    return rnd * (1.0 / 4294967296.0);
}

float RandomFloat(float minVal, float maxVal, inout uint seed)
{
    return lerp(minVal, maxVal, Random01(seed));
}

int RandomInt(int minVal, int maxVal, inout uint seed)
{
    float r = Random01(seed);
    return minVal + min((int) (r * (maxVal - minVal + 1)), maxVal - minVal);
}

float3 RandomDirection(inout uint seed)
{
    float z = Random01(seed) * 2.0 - 1.0;
    float a = Random01(seed) * 2.0 * PI;

    float r = sqrt(1.0 - z * z);

    return float3(
        r * cos(a),
        r * sin(a),
        z
    );
}

float3 RandomPointInSphere(float3 center, float radius, inout uint seed)
{
    float r = radius * pow(Random01(seed), 1.0 / 3.0);
    return center + RandomDirection(seed) * r;
}

cbuffer cbEmitterData : register(b0)
{
    uint particlesCount;
    uint time;
    float deltatime;
    float spawnRadius;

    float3 position;
    float speedMax;

    float speedMin;
    float scaleMax;
    float scaleMin;
    float lifetimeMax;

    float lifetimeMin;
    float respawnTimeMax;
    float respawnTimeMin;
    uint colorCount = 0;

    float4 particlesColor[MAX_COLOR];

    uint isEmitting;
};

RWStructuredBuffer<ParticleData> particles : register(u0);

[numthreads(256, 1, 1)]
void CS(uint3 ThreadId : SV_DispatchThreadID)
{
    uint index = ThreadId.x;
    
    if (index >= particlesCount)
        return;
    
    // Create Non-Created particles
    if (particles[index].isCreated == 0)
    {
        if (isEmitting == 0)
            return;
        
        uint seed = (index + 1u) * 747796405u ^ (time + 1u) * 2891336453u;

        particles[index].direction = RandomDirection(seed);
        particles[index].position = RandomPointInSphere(position, spawnRadius, seed);
        particles[index].velocity = RandomFloat(speedMin, speedMax, seed);
        particles[index].scale = RandomFloat(scaleMin, scaleMax, seed);
        particles[index].lifetime = RandomFloat(lifetimeMin, lifetimeMax, seed);
        particles[index].respawnTime = RandomFloat(respawnTimeMin, respawnTimeMax, seed);
        particles[index].color = particlesColor[RandomInt(0, colorCount - 1, seed)];
        
        particles[index].isCreated = 1;
        particles[index].isAlive = 0;
    }
    
    // Update Non-Alive particles
    if (particles[index].isAlive == 0)
    {
        if (isEmitting == 0)
            return;
        
        particles[index].respawnTime -= deltatime;
        
        if (particles[index].respawnTime <= 0.0)
            particles[index].isAlive = 1;
    }
    
    if (particles[index].isAlive == 1)
    {
        
        particles[index].position += particles[index].velocity * deltatime * particles[index].direction;
        particles[index].lifetime -= deltatime;
        
        if (particles[index].lifetime <= 0.0)
        {
            particles[index].isAlive = 0;
            particles[index].isCreated = 0;
        }
    }
}