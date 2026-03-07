#define MAX_COLOR 10
#define MAX_LIGHTS 32
#define MAX_TEXTURE_COUNT 64

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

struct Light
{
    uint type;
    float intensity;
    float spotInnerCos;
    float spotOuterCos;
    float3 color;
    float range;
    float3 direction;
    float pad0;
    float3 position;
    float pad1;
};

struct VertexIn
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

float4x4 MatrixFromPosition(float3 position)
{
    return float4x4(
    1.0, 0, 0, 0,
    0, 1.0, 0, 0,
    0, 0, 1.0, 0,
    position.x, position.y, position.z, 1.0);
}

float4x4 MatrixFromScale(float scale)
{
    return float4x4(
    scale, 0.0, 0.0, 0.0,
    0.0, scale, 0.0, 0.0,
    0.0, 0.0, scale, 0.0,
    0.0, 0.0, 0.0, 1.0);
}

Texture2D texturesList[MAX_TEXTURE_COUNT] : register(t0, space0);
SamplerState linearSample : register(s0, space0);

cbuffer cbPass : register(b0, space0)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float pad0;
    Light lights[MAX_LIGHTS];
    uint lightsCount;
    float3 ambientColor;
};

StructuredBuffer<ParticleData> particlesIn : register(t0, space1);

cbuffer Material : register(b0, space1)
{
    int albedoID;
    uint billBoardMode;
};

///////////////////////////////////////////////////
///// Cheating Billboard
///////////////////////////////////////////////////

VertexOut VS(VertexIn vin, uint index : SV_InstanceID)
{
    VertexOut vout;
    
    if (particlesIn[index].isAlive == 0)
    {
        vin.position = float3(0.0f, 0.0f, 0.0f);
    }

    float4x4 position = MatrixFromPosition(particlesIn[index].position);
    
    //Cheating Billboard inspired by : https://geeks3d.developpez.com/billboarding-vertex-shader/
    float4x4 posView = mul(position, view);
    
    if (billBoardMode != 0)
    {
        // Premiere colonne. 
        posView._11 = 1.0;
        posView._21 = 0.0;
        posView._31 = 0.0;
        
        if (billBoardMode == 1)
        {
            // Deuxieme colonne. 
            posView._12 = 0.0;
            posView._22 = 1.0;
            posView._32 = 0.0;
        }
        
        // Troiseme colonne. 
        posView._13 = 0.0;
        posView._23 = 0.0;
        posView._33 = 1.0;
    }
    
    float4 scaledLocalPos = mul(float4(vin.position, 1.0), MatrixFromScale(particlesIn[index].scale));
    float4 P = mul(scaledLocalPos, posView);
    vout.position = mul(P, proj);

    vout.color = particlesIn[index].color;
    vout.uv = vin.uv;

    return vout;
}

///////////////////////////////////////////////////
///// True Billboard
///////////////////////////////////////////////////

//VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
//{
//    VertexOut vout;

//    ParticleData p = particlesIn[instanceID];

//    if (p.isAlive == 0)
//    {
//        vout.position = float4(0, 0, 0, 0);
//        vout.color = float4(0, 0, 0, 0);
//        vout.uv = vin.uv;
//        return vout;
//    }

//    float3 particlePos = p.position;
//    float scale = p.scale;

//    float3 worldPos;

//    float3 localPos = vin.position * scale;

//    if (billBoardMode == 1)
//    {
//        float3 camRight = float3(view._11, view._21, view._31);
//        float3 camUp = float3(view._12, view._22, view._32);

//        worldPos = particlePos + camRight * localPos.x + camUp * localPos.y;
//    }
//    else if (billBoardMode == 2)
//    {
//        float3 look = normalize(cameraPos - particlePos);
//        look.y = 0;
//        look = normalize(look);

//        float3 right = float3(-look.z, 0, look.x);
//        float3 up = float3(0, 1, 0);

//        worldPos = particlePos + right * localPos.x + up * localPos.y;
//    }
//    else
//    {
//        worldPos = particlePos + localPos;
//    }

//    float4 viewPos = mul(float4(worldPos, 1.0f), view);
//    vout.position = mul(viewPos, proj);

//    vout.color = p.color;
//    vout.uv = vin.uv;

//    return vout;
//}

float4 PS(VertexOut pin) : SV_Target
{
    float4 albedoTexColor = float4(1.0, 1.0, 1.0, 1.0);
    if (albedoID >= 0)
    {
        albedoTexColor = texturesList[albedoID].Sample(linearSample, pin.uv);
    }
    
    float alpha = albedoTexColor.a * pin.color.a;
    
    float3 color = albedoTexColor.rgb * pin.color.rgb;
    
    clip(alpha - 0.5);
    
    return float4(color, alpha);
}