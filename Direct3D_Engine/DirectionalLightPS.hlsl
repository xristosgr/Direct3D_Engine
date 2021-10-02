#define NR_LIGHTS 2
cbuffer lightBuffer : register(b0)
{
    float4 dynamicLightPosition[NR_LIGHTS];
    float4 ambientLightColorAndStrength;
    float4 dynamicLightAttenuationAndStrength[NR_LIGHTS];
    float4 dynamicLightColor[NR_LIGHTS];
    
    float4 lightDirectionAndSpecularPower[NR_LIGHTS];
    float4 specularColor[NR_LIGHTS];
    
    
    float4 sunLightPosition;
    float4 sunLightColor;
    
    float4 sunlightDirectionAndSpecularPower;
    float4 sunspecularColor;
    
    bool enableLights;
    float3 padding;
    bool enableSunLight;
}


struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;

    
    
};

Texture2D objTextures  : register(t4);
Texture2D normalTextures  : register(t5);


SamplerState SampleType : register(s1);


float4 main(PS_INPUT input) : SV_TARGET
{
    float3 lightDir;
    float4 sampleColors = objTextures.Sample(SampleType, input.tex);
    float3 normalColors = normalTextures.Sample(SampleType, input.tex);

    
    float lightIntensity;
    
    lightDir = normalize(lightDirectionAndSpecularPower[0].xyz);

    
    lightIntensity = saturate(dot(normalColors.xyz, lightDir));
    float3 result = saturate(sampleColors.xyz * lightIntensity);

    
    return float4(result, 1.0f);
}
    
