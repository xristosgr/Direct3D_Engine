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
cbuffer cameraBuffer : register(b5)
{
    float4x4 proj;
    float4x4 view;
}


struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD0;
   
    float3 inWorldPos : WOLRD_POSITION;
    float4x4 viewMatrix : VIEW;
    float4 ViewPosition : TEXCOORD1;
    float4x4 projMatrix : PROJ;
    




    //float4 sunLightViewPosition : TEXCOORD5;
    //float4 lightViewPosition1 : TEXCOORD6;
    //float4 lightViewPosition2 : TEXCOORD7;
    
   

};

Texture2D DepthTexture : TEXTURE : register(t6);

Texture2D albedoRoughTexture : TEXTURE : register(t7);
Texture2D normalMetalTexture : TEXTURE : register(t8);
Texture2D positionTexture : TEXTURE : register(t9);


SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);


float4 main(PS_INPUT input) : SV_TARGET
{
    float2 projectTexCoord;
    projectTexCoord.x = input.ViewPosition.x / input.ViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.ViewPosition.y / input.ViewPosition.w / 2.0f + 0.5f;
    
    NR_LIGHTS = lightsNR;
    
    float outColor = DepthTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
    float3 color = 0.0f.xxx;
    float totalWeight = 0.0f;
    
    int xOffset = input.inTexCoord.x % 2 == 0 ? -1 : 1;
    int yOffset = input.inTexCoord.y % 2 == 0 ? -1 : 1;
    
    int2 offsets[] =
    {
        int2(0, 0),
        int2(0, yOffset),
        int2(xOffset, 0),
        int2(xOffset, yOffset)
    };
    
    for (int i = 0; i < 4; i++)
    {
        
    }
    
    return float4(outColor, outColor, outColor, 1);

}
