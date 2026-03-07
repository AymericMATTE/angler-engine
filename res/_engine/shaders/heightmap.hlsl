#define MAX_LIGHTS 32
#define LIGHT_DIR 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2
#define PI 3.14159265

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

cbuffer cbTransform : register(b0, space1)
{
    float4x4 world;
};

cbuffer cbMaterial : register(b1, space1)
{
    int albedoID;
    int heightID;
    float4 baseColor;
    float metallic;
    float roughness;
    float specular;
    float pad;
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
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};

VertexOut VS(VertexIn v) {
    VertexOut o;
    
    // Heightmap
    float3 displacedPosition = v.position.xyz;
    float height = 0.0f;
    if (albedoID >= 0) {
        float4 texSample = texturesList[heightID].SampleLevel(linearSample, v.uv, 0); // Apparently it's not possible to use Sample() in VS for some reason...
        height = dot(texSample.rgb, float3(0.299, 0.587, 0.114)); // Luminance from : https://github.com/mrdooz/kumi/blob/master/effects/luminance.hlsl
    }

    // Apply
    float heightStrength = 85.0f;
    displacedPosition.y += height * heightStrength;

    float4 worldPos = mul(float4(displacedPosition, 1.0f), world);
    o.worldPos = worldPos.xyz;

    float4 viewPos = mul(worldPos, view);
    o.position = mul(viewPos, proj);
    
    o.normal = mul(v.normal, (float3x3)world);
    
    o.color = v.color;
    o.uv = v.uv;

    return o;
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

// Schlick Fresnel
// I took it from this stackoverflow post : https://gamedev.stackexchange.com/questions/118384/schlick-fresnel-shader
// And adapted it from GLSL to HLSL, using the variables described in another paper
float3 F_Schlick(float3 F0, float cosTheta) {
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

// GGX Normal Distribution
// Took from this article : https://learnopengl.com/PBR/Theory
// Again, converted from GLSL to HLSL with some var changes and stuff..
float D_GGX(float nh, float a) { 
    float a2 = a * a;
    float denom = nh * nh * (a2 - 1.0f) + 1.0f;
    return a2 / (PI * denom * denom);
}

// Smith GGX Geometry
// Stolen from this article : https://github.khronos.org/Vulkan-Site/tutorial/latest/Building_a_Simple_Engine/Lighting_Materials/04_lighting_implementation.html
// Again, converted from GLSL, blablabla you get it..
float G_SmithGGX(float nv, float nl, float a) {
    float k = (a * a) / 2.0f; // Direct lighting version
    
    float Gv = nv / (nv * (1.0f - k) + k);
    float Gl = nl / (nl * (1.0f - k) + k);
    return Gv * Gl;
}

float4 PS(VertexOut p) : SV_Target {
    const float minimumRoughness = 0.04f;
    const float epsilon = 0.0001f;

    float3 N = normalize(p.normal);
    float3 V = normalize(cameraPos - p.worldPos);
    float nv = saturate(dot(N, V));

    // If a texture is provided, mix it's pixel color with the vertex color & object color :)
    float4 albedoTexColor = float4(1.0, 1.0, 1.0, 1.0);
    if(albedoID >= 0) {
        albedoTexColor = texturesList[albedoID].Sample(linearSample, p.uv);
    }
    float3 albedo = baseColor.rgb * p.color.rgb * albedoTexColor.rgb;
    
    float a = max(roughness * roughness, minimumRoughness);
    float m = saturate(metallic);

    // F0 for dielectrics is 0.04, for metals this the albedo
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedo, m);
    
    float3 diffuseColor = lerp(albedo, float3(0.0f, 0.0f, 0.0f), m); // Metals have no diffuse, dielectrics do

    float3 Lo = 0.0f;

    for(uint i = 0; i < lightsCount; i++) {
        Light Ld = lights[i];

        float3 L;
        float atten = 1.0f;

        if(Ld.type == LIGHT_DIR) {
            L = normalize(-Ld.direction);
        }
        else {
            float3 d = Ld.position - p.worldPos;
            float dist = length(d);
            L = d / max(dist, epsilon);

            // Inverse square attenuation
            atten = 1.0f / (1.0f + dist * dist);

            if(Ld.type == LIGHT_SPOT) {
                float cosTheta = dot(-L, normalize(Ld.direction));
                float spot = smoothstep(Ld.spotOuterCos, Ld.spotInnerCos, cosTheta);
                atten *= spot;
            }
        }

        float nl = saturate(dot(N, L));
        if(nl <= 0.0f)
            continue;

        float3 H = normalize(V + L);
        float nh = saturate(dot(N, H));
        float vh = saturate(dot(V, H));
        float lh = saturate(dot(L, H));

        // Fresnel
        float3 F = F_Schlick(F0, lh);
        
        // Normal Distribution
        float D = D_GGX(nh, a);
        
        // Geometry
        float G = G_SmithGGX(nv, nl, a);

        // Cook-Torrance specular
        float3 specular = (D * F * G) / max(4.0f * nv * nl, epsilon); // https://learnopengl.com/PBR/Lighting
        
        // Lambertian diffuse (only for non-metals)
        float3 diffuse = diffuseColor / PI; // NGL I asked ChatGPT i have no idea why the Lambertian law is even useful

        // Energy conservation
        float3 kD = (float3(1.0f, 1.0f, 1.0f) - F) * (1.0f - m);
        
        Lo += (kD * diffuse + specular) * Ld.color * Ld.intensity * atten * nl;
    }
    
    // This simulates indirect specular for metals
    float3 ambientFresnel = F_Schlick(F0, nv);
    float3 ambient = ambientColor * lerp(diffuseColor, F0, ambientFresnel);
    
    float3 color = Lo + ambient;

    // Tone mapping
    color = ACESFilm(color);
    
    float alpha = baseColor.a * p.color.a * albedoTexColor.a;
    
    clip(alpha - 0.5); // Alpha clipping cauz i didnt had the time to implement alpha blending

    return float4(color, alpha);
}