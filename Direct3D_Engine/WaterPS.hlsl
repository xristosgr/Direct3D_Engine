#include "ShaderSettings.hlsli"
cbuffer waterBuffer : register(b2)
{

    float waterTranslation;
}

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
    
    float3 inTangentLightPos : TEXCOORD5;
    float3 inTangentViewPos : TEXCOORD6;
    float3 inTangentFragPos : TEXCOORD7;
   
    float heightData : TEXCOORD8;

};

static const float PI = 3.14159265359;

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D roughnessTexture : TEXTURE : register(t2);
Texture2D metallicTexture : TEXTURE : register(t3);
Texture2D shadowTexture : TEXTURE : register(t4);
//Texture2D ssrTexture : TEXTURE : register(t5);
TextureCube environmentTexture : TEXTURE : register(t5);
Texture2D bloomTexture : TEXTURE : register(t6);
Texture2D aoTexture : TEXTURE : register(t7);
//Texture2D ambientTexture : TEXTURE : register(t8);
Texture2D blackAndWhiteTexture : TEXTURE : register(t9);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

float3 fresnelSchlick(float cosTheta, float3 F0);
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float Shadows(float4 viewPos);
float3 shadowLights(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index);
float3 sunLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0);
float3 sunLight2(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0);


float4 main(PS_INPUT input) : SV_TARGET
{
    float3 viewDir = normalize(input.inTangentViewPos - input.inTangentFragPos);

    float2 normalTexCoord = input.inTexCoord;
   // input.inTexCoord.y += (waterTranslation);
   // normalTexCoord.y += (waterTranslation);
   

    float4 albedo = float4(pow(objTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz, 2.2f), 1.0f);

 
   // float x = -1;
   // clip(x);
  //
  //  float roughness = roughnessTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
  //  float metallic = metallicTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
  // // roughness = 0.3;
  //  metallic = 0.0f;
  //
  // // float x = -1;
  // // clip(x);
  //  float3 normalColor = normalTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;
  //  float3 normalColor2 = normalTexture.Sample(SampleTypeWrap, normalTexCoord).xyz;
  //
  //  float ao = aoTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
  // 
  //  
  //  normalColor = (normalColor * 2.0f) - 1.0f;
  //  normalColor2 = (normalColor2 * 2.0f) - 1.0f;
  //  float3 bumpNormal = (normalColor.x * normalColor2.x * input.inTangent) + (normalColor.y * normalColor2.y * input.inBinormal) + (normalColor.z * normalColor2.z * input.inNormal);
  //  bumpNormal = normalize(bumpNormal);
  //  float3 V = normalize(cameraPos - input.inWorldPos);
  //
  //
  //  
  //  float3 F0 = float3(0.04f, 0.04f, 0.04f);
  //  F0 = lerp(F0, albedo.rgb, metallic);
  //
  //  float3 Lo = float3(0.0f, 0.0f, 0.0f);
  //  
  //  if (enableSunLight)
  //      Lo += sunLight(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0);
  //  for (int i = 0; i < NR_LIGHTS; i++)
  //  {
  //      if (enabledLights[i].x == 1)
  //      {
  //          Lo += shadowLights(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0, i);
  //      }
  //      
  // 
  //
  //  }
  //  //float3 F = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f), F0, roughness);
  //  //float3 kS = F;
  //  //float3 kD = 1.0f - kS;
  //  //kD *= 1.0f - metallic;
  //  
  //  float3 ambient = albedo.rgb * sunLightColor.xyz*ao;
  //  
  //  float3 color = ambient + Lo;
  //  
  //  float3 I = normalize(input.inWorldPos - cameraPos);
  //  float3 R = reflect(I, normalize(bumpNormal));
  //  
  //  float3 refectionMap = ((environmentTexture.Sample(SampleTypeWrap, R)) * roughness)*2;
  //  
  //  float2 projectTexCoord;
  //  projectTexCoord.x = input.ViewPosition.x / input.ViewPosition.w / 2.0f + 0.5f;
  //  projectTexCoord.y = -input.ViewPosition.y / input.ViewPosition.w / 2.0f + 0.5f;
  //
  //
  //  float3 bloom = bloomTexture.Sample(SampleTypeWrap, projectTexCoord);
  //  
  // // color += bloom;
  //  color = color / (color + float3(1.0, 1.0f, 1.0f));
  //  color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
  //  
  // 
  // 
  //  color += refectionMap;
   //color *= Shadows(input.ViewPosition);
 
  
    return float4(albedo.rgb, 1.0f);

    
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    
    denom = PI * denom * denom;
    return nom / max(denom, 0.001f);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    
    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;
    
    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}


float Shadows(float4 viewPos)
{
    float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;

     
    projectTexCoord.x = viewPos.x / viewPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -viewPos.y / viewPos.w / 2.0f + 0.5f;
 

    depthValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r;
    //lightDepthValue = viewPos.z / viewPos.w;
    //
    //lightDepthValue = lightDepthValue - bias;
    
    
    return depthValue;
   
}

float3 shadowLights(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index)
{
    float3 L = normalize(dynamicLightPosition[index].xyz - input.inWorldPos.xyz).xyz;
    float3 H = normalize(V + L);
        
    float distance = length(dynamicLightPosition[index].xyz - input.inWorldPos.xyz);
    float attenuation = 1.0f / (distance * distance);
    float3 radiance = dynamicLightColor[index] * attenuation;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);


        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f)+0.001;
    float3 specular = nominator / max(denominator, 0.001f);
        
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
        
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 sunLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0)
{
    float3 lightDir = sunlightDirectionAndSpecularPower.xyz;

   // if (sunlightDirectionAndSpecularPower.y < 0)
   // {
   //     lightDir.y = -sunlightDirectionAndSpecularPower.y;
   //
   // }
   // else if (sunlightDirectionAndSpecularPower.y == 0)
   // {
   //     lightDir.y = 0.01f;
   //
   // }
 
    
   // float3 L = normalize(sunLightPosition.xyz - input.inWorldPos.xyz).xyz;
    float3 L = normalize( lightDir);
    float3 H = normalize(V + L);
        
    
    float3 radiance = sunLightColor.xyz;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f)+0.001;
    float3 specular = nominator / denominator * sunlightDirectionAndSpecularPower.w;
        
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
        
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);

}