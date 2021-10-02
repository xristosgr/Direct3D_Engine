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
    
    float4 enabledLightsAttribs[MAX_NO_OF_LIGHTS];
    
    
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

cbuffer lightGlobalBuffer : register(b8)
{
    unsigned int lightsSize;
   // unsigned int shadowLightSize;
}



struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
    float4 lightViewPosition[MAX_NO_OF_SHADOWS] : LIGHTVIEWS;

 
};

Texture2D sampleTexture : TEXTURE : register(t0);


Texture2D depthMapTextures[MAX_NO_OF_SHADOWS] : TEXTURE : register(t4);



SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);


float Shadows(float4 lightViewPosition, Texture2D depthMapTexture);


static float3 diffuseLightIntensity = 0.0f;

//static float3 lightCombine;

float4 main(PS_INPUT input) : SV_TARGET
{
    //float4 sampleColor = sampleTexture.Sample(SampleTypeWrap, input.inTexCoord);
    //if (sampleColor.a<0.1f)
    //{
    //    discard;
    //}
    float3 appliedLight = float3(0, 0, 0);

    
    if (enableSunLight)
    {
       // for (int i = 0; i < 4;++i)
       // {
       //     appliedLight *= SunShadows(input.cascadeViewPosition[i], sunDepthMapTexture, i);
       // }
    
    }
    int index = 0;
 
    if (EnableLights)
    {
        if (enableShadows)
        {
            for (int i = 0; i < MAX_NO_OF_SHADOWS; ++i)
            {
                if (i < lightsSize)
                {
                    if (enabledLightsAttribs[i].x == 1 && enabledLightsAttribs[i].y == 1)
                    {
          
                        appliedLight += float3(Shadows(input.lightViewPosition[i], depthMapTextures[i]), Shadows(input.lightViewPosition[i], depthMapTextures[i]), Shadows(input.lightViewPosition[i], depthMapTextures[i]));
                    }
                }
                else
                {
                    break;
                }
               
            }
            
        }
     
    }

    return saturate(float4(appliedLight, 1.0f));
    
}


float Shadows(float4 lightViewPosition, Texture2D depthMapTexture)
{
    //float zbias = bias;
    float2 projectTexCoord;
    float lightDepthValue;
    float shadow = 0.0f;
    
    int width;
    int height;
    depthMapTexture.GetDimensions(width, height);
    float2 texelSize;
    texelSize.x = 0.5 / width;
    texelSize.y = 0.5 / height;
    
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w;
   
    projectTexCoord.x = projectTexCoord.x * 0.5 + 0.5;
    projectTexCoord.y = projectTexCoord.y * 0.5 + 0.5;
    
  
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {

        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        
       
        int PCF_RANGE = 1;
        lightDepthValue = lightDepthValue - bias;
        
        [unroll]
        for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
        {
            [unroll]
            for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
              {
                   float pcfDepth = depthMapTexture.Sample(SampleTypeWrap, projectTexCoord + float2(x, y) * texelSize).r;
                
                   shadow += (lightDepthValue < pcfDepth);
              }
        }
        shadow /= ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
        
        
     
    }
    else
    {
       // shadow = 1.0f;
    }
  
    return shadow;
}

