#define MAX_TEXTURE_COUNT 64
#define SAMPLER_MAX_COUNT 2
#define SAMPLER_LINEAR_WRAP 0
#define SAMPLER_LINEAR_CLAMP 1

Texture2D texturesList[MAX_TEXTURE_COUNT] : register(t0, space0);
SamplerState linearSample[SAMPLER_MAX_COUNT] : register(s0, space0);

cbuffer cbPassData : register(b0, space0)
{
    uint textureID;
    uint extractColors;
    uint horizontalPass;
    uint verticalPass;
    float2 texelSize; // (1 / targetWidth, 1 / targetHeight)
    float padding;
};

struct VertexIn
{
    float2 position : POSITION;
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
    vertexOut.position = float4(vertexIn.position, 0.0, 1.0);
    vertexOut.uv = vertexIn.uv;
    return vertexOut;
}

// Luminance from : https://github.com/mrdooz/kumi/blob/master/effects/luminance.hlsl
float luminance(float3 _color) {
    return dot(_color, float3(0.299, 0.587, 0.114));
}

// ACES Tonemapper (my beloved)
// I took it from this article : https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 ACESFilm(float3 x) {
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x*(a*x+b))/(x*(c*x+d)+e));
}

float4 PS(VertexOut pixelIn) : SV_Target {
    // Weights for the Gaussian blur kernel
    float weights[9] = {
        0.016216f,
        0.054054f,
        0.121621f,
        0.194594f,
        0.227027f,
        0.194594f,
        0.121621f,
        0.054054f,
        0.016216f
    };

    float2 uv = pixelIn.uv;
    float4 finalColor = float4(0,0,0,0);

    // Extract pixels based on luminance (luminance func not by me) 
    if (extractColors == 1) {
        finalColor = texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv);

        if (luminance(finalColor.rgb) <= 0.85) // TO DO : Expose to the user
            finalColor = float4(0,0,0,1);

        return finalColor;
    }
    
    // Blur horizontaly
    if (horizontalPass == 1) {
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(-4 * texelSize.x, 0)) * weights[0];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(-3 * texelSize.x, 0)) * weights[1];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(-2 * texelSize.x, 0)) * weights[2];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(-1 * texelSize.x, 0)) * weights[3];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv)                               * weights[4];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2( 1 * texelSize.x, 0)) * weights[5];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2( 2 * texelSize.x, 0)) * weights[6];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2( 3 * texelSize.x, 0)) * weights[7];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2( 4 * texelSize.x, 0)) * weights[8];

        return finalColor;
    }

    // Blur verticaly
    if (verticalPass == 1) {
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0, -4 * texelSize.y)) * weights[0];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0, -3 * texelSize.y)) * weights[1];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0, -2 * texelSize.y)) * weights[2];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0, -1 * texelSize.y)) * weights[3];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv)                               * weights[4];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0,  1 * texelSize.y)) * weights[5];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0,  2 * texelSize.y)) * weights[6];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0,  3 * texelSize.y)) * weights[7];
        finalColor += texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv + float2(0,  4 * texelSize.y)) * weights[8];

        return finalColor;
    }
    
    // Final composition, TO DO : perform in another shader
    float4 sceneColor = texturesList[0].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv);
    float4 bloomColor = texturesList[textureID].Sample(linearSample[SAMPLER_LINEAR_CLAMP], uv);
    
    float3 combined = sceneColor.rgb + bloomColor.rgb;
    
    combined = ACESFilm(combined);

    return float4(combined, sceneColor.a);
}