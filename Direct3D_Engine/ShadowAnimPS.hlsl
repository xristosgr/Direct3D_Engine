
#include "ShaderSettings.hlsli"

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    
    float4 ViewPosition : TEXCOORD1;


    float3 playerViewDir : TEXCOORD2;
    float3 playerViewDirSpecular : TEXCOORD3;
    
    float4 depthPosition : TEXTURE4;
    //float4 cascadeViewPosition1 : TEXCOORD5;
    //float4 cascadeViewPosition2 : TEXCOORD6;
    //float4 cascadeViewPosition3 : TEXCOORD7;
    //float4 cascadeViewPosition4 : TEXCOORD8;
   // float4 cascadeViewPosition[4] : SUNVIEWS;
   //float4 lightViewPosition1 : TEXCOORD9;
   //float4 lightViewPosition2 : TEXCOORD10;
    float4 lightViewPosition[2] : LIGHTVIEWS;
 
};
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

Texture2D sampleTexture : TEXTURE : register(t0);


Texture2DArray depthMapTextures : TEXTURE : register(t4);
//Texture2DArray depthMapTexture : TEXTURE : register(t5);
//Texture2D depthMapTexture5 : TEXTURE : register(t5);
//Texture2D depthMapTexture6 : TEXTURE : register(t6);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

float3 Shadows(float4 lightViewPosition, Texture2DArray depthMapTexture, int index);
float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture);
float3 DirectionalShadows(float4 lightViewPosition, Texture2D depthMapTexture);

static float3 diffuseLightIntensity = 0.0f;

//static float3 lightCombine;

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 sampleColor = sampleTexture.Sample(SampleTypeWrap, input.inTexCoord);
    if (sampleColor.a < 0.1f)
    {
        discard;
    }
    float3 appliedLight = float3(1, 1, 1);
 
    if (enableSunLight)
    {
       // for (int i = 0; i < 4; ++i)
       // {
       //     appliedLight *= Shadows(input.cascadeViewPosition[i], sunDepthMapTexture, i);
       // }
    
    }

    if (EnableLights)
    {
        for (int i = 0; i < 2; ++i)
        {
            appliedLight *= Shadows(input.lightViewPosition[i], depthMapTextures, i);
        }
      // appliedLight *= Shadows(input.lightViewPosition[0], depthMapTexture5);
      // appliedLight *= Shadows(input.lightViewPosition[1], depthMapTexture6);
    }

    return saturate(float4(appliedLight , 1.0f));
    
}


float3 Shadows(float4 lightViewPosition, Texture2DArray depthMapTexture, int index)
{
    float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    
    
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
   

    
    float3 appliedLight = float3(ambientLightColorAndStrength.w, ambientLightColorAndStrength.w, ambientLightColorAndStrength.w);
    
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
     
        depthValue = depthMapTexture.Sample(SampleTypeClamp, float3(projectTexCoord, index)).r;
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    
        lightDepthValue = lightDepthValue - bias;
     
        if (lightDepthValue < depthValue)
        {
            
            appliedLight = float3(1.0f, 1.0f, 1.0f);
         
        }
    }
    else
    {
        appliedLight = float3(1.0f, 1.0f, 1.0f);
    }
    return appliedLight;
}
float3 Shadows(float4 lightViewPosition, Texture2D depthMapTexture)
{
    float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    
    
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
   

    
    float3 appliedLight = float3(ambientLightColorAndStrength.w, ambientLightColorAndStrength.w, ambientLightColorAndStrength.w);
    
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
     
        depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    
        lightDepthValue = lightDepthValue - bias;
     
        if (lightDepthValue < depthValue)
        {
            
            appliedLight = float3(1.0f, 1.0f, 1.0f);
         
        }
    }
    else
    {
        appliedLight = float3(1.0f, 1.0f, 1.0f);
    }
    return appliedLight;
}
float3 DirectionalShadows(float4 lightViewPosition, Texture2D depthMapTexture)
{
    float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    
 
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;
   

    
    float3 appliedLight = float3(ambientLightColorAndStrength.w, ambientLightColorAndStrength.w, ambientLightColorAndStrength.w);
    
    
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    
        lightDepthValue = lightDepthValue - bias;
     
        if (lightDepthValue < depthValue)
        {
            appliedLight = float3(1.0f, 1.0f, 1.0f);
        }
     
    }
    else
    {
        appliedLight = float3(1.0f, 1.0f, 1.0f);
    }
    return appliedLight;
}
