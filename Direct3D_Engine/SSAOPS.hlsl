#include "ShaderSettings.hlsli"

cbuffer lightBuffer : register(b0)
{
    float4 dynamicLightPosition[MAX_NO_OF_LIGHTS];
    float4 ambientLightColorAndStrength;
    float4 dynamicLightAttenuationAndStrength[MAX_NO_OF_LIGHTS];
    float4 dynamicLightColor[MAX_NO_OF_LIGHTS];
    
    float4 lightDirectionAndSpecularPower[MAX_NO_OF_LIGHTS];
    float4 specularColor[MAX_NO_OF_LIGHTS];
    
    
    float4 sunLightPosition;
    float4 sunLightColor;

    float4 sunlightDirectionAndSpecularPower;
    float4 sunspecularColor;
    
    
 
    
    int lightsNR;
    float3 padding;
    
    float4 enabledLights[MAX_NO_OF_LIGHTS];
    
    
    bool EnableLights;
    float3 cameraPos;
    bool enableSunLight;

}
cbuffer PCFbuffer : register(b3)
{
    int pcfLevel;
    double bias;
    bool enableShadows;
}
cbuffer lightCull : register(b4)
{
    float4 Radius1[2];

}
cbuffer ssaoBuffer : register(b5)
{
    float4x4 projection;
    float4 ssaoKernel[64];

};

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float4 ViewPosition : TEXCOORD1;
    float4x4 projMatrix : TEXCOORD2;

};

struct PixelOutputType
{
    float worldPos : SV_Target0;
};

Texture2D normalTexture : TEXTURE : register(t7);
Texture2D positionTexture : TEXTURE : register(t8);
Texture2D randomVecTexture : TEXTURE : register(t9);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);
SamplerState SampleTypessao : register(s2);

PixelOutputType main(PS_INPUT input) : SV_TARGET
{
    PixelOutputType output;
    
    float2 projectTexCoord;

    const float2 noiseScale = float2((float)window_width / 4.0, (float)window_height / 4.0); // screen = 800x600
    
    
    float3 fragPos = positionTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;
    float3 normal = normalTexture.Sample(SampleTypeWrap, input.inTexCoord).rgb;
    float3 randomVec = randomVecTexture.Sample(SampleTypeWrap, input.inTexCoord * noiseScale).xyz;
    float3 tangent = (randomVec - normal * dot(randomVec, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    
    int kernelSize = 64;
    float radius = 0.5f;
    float bias = 0.025f;
    
    float occlusion = 0.0;
    for (int i = 0; i < 64; ++i)
    {
        // get sample position
        float3 samplePos = mul(ssaoKernel[i].xyz, TBN);// from tangent to view-space
        samplePos = fragPos + samplePos * radius;
        
        // project sample position (to sample texture) (to get position on screen/texture)
        float4 offset = float4(samplePos, 1.0);
        offset = mul(offset, input.projMatrix); // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = positionTexture.Sample(SampleTypeWrap, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    output.worldPos = occlusion;
    
    return output;
}


