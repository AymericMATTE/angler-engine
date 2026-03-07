#define MAX_TEXTURE_COUNT 64

Texture2D texturesList[MAX_TEXTURE_COUNT] : register(t0, space0);
SamplerState linearSample : register(s0, space0);

cbuffer cbPassData : register(b0, space0)
{
    float4x4 proj;
};

cbuffer cbPerObject : register(b0, space1)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
};

cbuffer cbMaterial : register(b1, space1)
{
    float4 color;
    int textureID;
};

struct VertexIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexOut VS(VertexIn vertexIn)
{
    VertexOut vertexOut;
    float4x4 worldProj = mul(gWorld, proj);
    vertexOut.position = mul(float4(vertexIn.position, 1.0f), worldProj);
    vertexOut.uv = mul(float4(vertexIn.uv, 0.0f, 1.0f), gTexTransform).xy;
   
    return vertexOut;
}

float4 PS(VertexOut pixelIn) : SV_Target
{
    if (textureID < 0)
    {
        clip(color.a - 0.01f);
        return color;
    }
    float4 finalColor = texturesList[textureID].Sample(linearSample, pixelIn.uv) * color;
    clip(finalColor.a - 0.01f);
    return finalColor;
}