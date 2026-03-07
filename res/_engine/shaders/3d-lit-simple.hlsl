#define MAX_LIGHTS 32
#define LIGHT_DIR 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

struct Light {
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
    float3 cameraPos;
    float pad0;
    Light lights[MAX_LIGHTS];
    uint lightsCount;
    float3 ambientColor;
};

cbuffer cbTransform : register(b0, space1)
{
    float4x4 world;
};

cbuffer cbMaterial : register(b1, space1)
{
    int albedoID;
    float4 baseColor;
    float metallic;
    float roughness;
};

struct VertexIn {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOut {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1;
};

VertexOut VS(VertexIn vertexIn) {
    VertexOut vertexOut;

    float4 worldPos = mul(float4(vertexIn.position, 1.0f), world);
    float4 viewPos  = mul(worldPos, view);
    vertexOut.position = mul(viewPos, proj);

    vertexOut.worldPos = worldPos.xyz;

    vertexOut.color = vertexIn.color;
    vertexOut.normal = mul(vertexIn.normal, (float3x3)world);
    
    vertexOut.uv = vertexIn.uv;

    return vertexOut;
}

// This is just a little trick to reuse the 'ambientColor' param & turn it into the 'ambientStrength' needed for Phong.
float avg(float3 input) {
    return (input.x + input.y + input.z) / 3.0;
}

// Apparently HLSL does not have a built in reflect function, so I took it here : https://asawicki.info/news_1301_reflect_and_refract_functions.html
float3 reflect(float3 incident, float3 normal) {
    return incident - 2.0 * dot(incident, normal) * normal;
}

float3 phong(float3 normal, float3 viewDir, float3 lightDir, float3 lightColor,
    const float ambientStrength, const float metallic, const float roughness) {
    
    // Compute shininess (I cheat a little bit because i call it metallic and 1- it..)
    const float minimumShininess = 16.0f;
    const float maximumShininess = 256.0;
    const float shininess = lerp(minimumShininess, maximumShininess, (1 - metallic));
    
    // Compute roughness (Again I cheat to make it compatible with the params of the other lighting model)
    const float smoothness = (1 - roughness);
    
    // Compute diffuse
    float diffIntensity = max(dot(normal, lightDir), 0.0);
    float3 diffuse = diffIntensity * lightColor;
    
    // Compute specular
    float3 reflectDir = reflect(-lightDir, normal);
    float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float3 specular = smoothness * specIntensity * lightColor;
    
    // Some basic ambient with little control..
    float3 ambient = ambientStrength * lightColor;
    
    return (ambient + diffuse + specular); // Yep that's pretty much it.. You gotta love phong when you are bad at math like me :)
}

float4 PS(VertexOut pixelIn) : SV_Target {
    float3 pixelNormal = normalize(pixelIn.normal);
    float3 pixelToView = normalize(cameraPos - pixelIn.worldPos);
    
    // If a texture is provided, mix it's pixel color with the vertex color & object color :)
    float4 albedoTexColor = float4(1.0, 1.0, 1.0, 1.0);
    if(albedoID >= 0) {
        albedoTexColor = texturesList[albedoID].Sample(linearSample[SAMPLER_LINEAR_WRAP], pixelIn.uv);
    }
    float4 objectColor = baseColor * pixelIn.color * albedoTexColor;

    clip(objectColor.a - 0.01f);
    
    const float ambientStrength = avg(ambientColor);

    float3 accumulatedIrradiance = float3(0.0, 0.0, 0.0);

    for (uint i = 0; i < lightsCount; i++) {
        float3 lightColor = lights[i].color * lights[i].intensity;
        float3 lightDir = float3(0.0, 0.0, 0.0);

        if (lights[i].type == LIGHT_DIR) {
            lightDir = normalize(-lights[i].direction);
        }
        else if (lights[i].type == LIGHT_POINT) {
            lightDir = normalize(lights[i].position - pixelIn.worldPos);
            
            // It's maybe a bit more accurate, tho it look bad
            float distance = length(lights[i].position - pixelIn.worldPos);
            float attenuation = saturate(1.0 - distance / (lights[i].range * 4.0));
            lightColor *= attenuation;
        }
        else if (lights[i].type == LIGHT_SPOT) {
            lightDir = normalize(lights[i].position - pixelIn.worldPos);
            
            // Fancy maths for calculating the cone
            float theta = dot(lightDir, normalize(-lights[i].direction));
            float epsilon = lights[i].spotInnerCos - lights[i].spotOuterCos;
            float intensity = saturate((theta - lights[i].spotOuterCos) / epsilon);

            // Also look bad, but kind of needed for spot lights
            float distance = length(lights[i].position - pixelIn.worldPos);
            float attenuation = saturate(1.0 - distance / lights[i].range * 4.0);
            lightColor *= attenuation * intensity;
        }

        // Diffuse + Specular + Ambient
        float3 lighting = phong(
            pixelNormal,
            pixelToView,
            lightDir,
            lightColor * 0.25,
            ambientStrength,
            metallic,
            roughness);

        // Add to the total received ammount of light (yeah i like fancy names)
        accumulatedIrradiance += lighting;
    }
    
    clip(objectColor.a - 0.5); // Alpha clipping cauz i didnt had the time to implement alpha blending

    return float4(finalColor, objectColor.a);
}