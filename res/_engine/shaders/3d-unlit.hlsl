#define MAX_TEXTURE_COUNT 64
#define SAMPLER_MAX_COUNT 2
#define SAMPLER_LINEAR_WRAP 0
#define SAMPLER_LINEAR_CLAMP 1

Texture2D texturesList[MAX_TEXTURE_COUNT] : register(t0, space0);
SamplerState linearSample[SAMPLER_MAX_COUNT] : register(s0, space0);

cbuffer cbPass : register(b0, space0)
{
    float4x4 view;
    float4x4 proj;
};

cbuffer cbTransform : register(b0, space1)
{
    float4x4 world;
};

cbuffer cbMaterial : register(b1, space1)
{
    int albedoID;
    float4 baseColor;
};

struct VertexIn {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VertexOut {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

VertexOut VS(VertexIn vertexIn) {
    VertexOut vertexOut;
    
    float4 worldPos = mul(float4(vertexIn.position, 1.0f), world);
    float4 viewPos  = mul(worldPos, view);
    
    vertexOut.position = mul(viewPos, proj);
    vertexOut.color = vertexIn.color;
    vertexOut.uv = vertexIn.uv;
   
    return vertexOut;
}

float4 PS(VertexOut pixelIn) : SV_Target {
    float4 albedoTexColor = float4(1.0, 1.0, 1.0, 1.0);
    if(albedoID >= 0) {
        albedoTexColor = texturesList[albedoID].Sample(linearSample[SAMPLER_LINEAR_WRAP], pixelIn.uv);
    }
    
    return pixelIn.color * baseColor * albedoTexColor;
}