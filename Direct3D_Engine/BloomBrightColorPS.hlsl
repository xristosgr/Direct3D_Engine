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
    //float4 sunLightViewPosition : TEXCOORD5;
    //float4 lightViewPosition1 : TEXCOORD6;
    //float4 lightViewPosition2 : TEXCOORD7;
    
   

};

Texture2D inputTexture : TEXTURE : register(t4);

SamplerState objSamplerStateClamp : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);


float4 main(PS_INPUT input) : SV_TARGET
{

    float3 sampleColor = inputTexture.Sample(objSamplerState, input.inTexCoord).xyz;
    float brightness = dot(sampleColor, float3(0.2126, 0.7152, 0.0722));
    
    if(brightness > 1.0f)
    {
        
        sampleColor = sampleColor / (sampleColor + float3(1.0, 1.0f, 1.0f));
        sampleColor = pow(sampleColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
        return (float4(sampleColor * ambientLightColorAndStrength.w*100, 1.0f));
        
    }
    else
    {
        return (float4(0, 0, 0, 1.0f));
    }

}