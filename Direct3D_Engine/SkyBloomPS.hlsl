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
cbuffer skyBuffer : register(b1)
{
    float4 apexColor;
    float4 centerColor;

}

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 domePosition : TEXCOORD0;
    float4 ViewPosition : TEXCOORD1;
};

//static float4 apexColor = float4(0.0f, 0.05f, 0.6f, 1.0f);
//static float4 centerColor = float4(0.0f, 0.05, 0.8f, 1.0f);
float4 main(PS_INPUT input) : SV_TARGET
{
    float height;
    
    height = input.domePosition.y;
    if (height < 0.0)
    {
        height = 0.0f;
    }

    
    
    float4 OutPutColor = lerp(centerColor, apexColor, height);
    

    //OutPutColor.rgb += OutPutColor;
    //OutPutColor.rgb = OutPutColor.rgb / (OutPutColor.rgb + float3(1.0, 1.0f, 1.0f));
    //OutPutColor.rgb = pow(OutPutColor.rgb, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    return OutPutColor;
}