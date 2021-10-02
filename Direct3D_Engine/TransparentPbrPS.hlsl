#include "ShaderSettings.hlsli"


cbuffer lightBuffer : register(b0)
{
    float4 ambientLightColorAndStrength;
    float4 dynamicLightPosition[MAX_NO_OF_LIGHTS];

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
cbuffer lightCull : register(b4)
{
    float4 Radius1[MAX_NO_OF_LIGHTS];
    float4 Radius2[MAX_NO_OF_LIGHTS];
}
cbuffer NoShadowLightBuffer : register(b7)
{
    float4 _dynamicLightPosition[MAX_NO_OF_LIGHTS];
    float4 _ambientLightColorAndStrength;
    float4 _dynamicLightAttenuationAndStrength[MAX_NO_OF_LIGHTS];
    float4 _dynamicLightColor[MAX_NO_OF_LIGHTS];
    
    float4 _lightDirectionAndSpecularPower[MAX_NO_OF_LIGHTS];
    float4 _specularColor[MAX_NO_OF_LIGHTS];
    
    
    float4 _sunLightPosition;
    float4 _sunLightColor;

    float4 _sunlightDirectionAndSpecularPower;
    float4 _sunspecularColor;
    
    
 
    
    int _lightsNR;
    float3 _padding;
    
    float4 _enabledLights[MAX_NO_OF_LIGHTS];
    
    
    bool _EnableLights;
    float3 _cameraPos;
    bool _enableSunLight;

}

cbuffer lightGlobalBuffer : register(b8)
{
    unsigned int lightsSize;
   // unsigned int shadowLightSize;
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
    
    float4 lightViewPosition[MAX_NO_OF_SHADOWS] : LIGHTVIEWS;
   

};

static const float PI = 3.14159265359;


Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D roughnessTexture : TEXTURE : register(t2);
Texture2D metallicTexture : TEXTURE : register(t3);
TextureCube irradianceMap : TEXTURE : register(t4);
Texture2D brdfLUT : TEXTURE : register(t5);
TextureCube prefilterMap1 : TEXTURE : register(t6);
TextureCube prefilterMap2 : TEXTURE : register(t7);
TextureCube prefilterMap3 : TEXTURE : register(t8);
TextureCube prefilterMap4 : TEXTURE : register(t9);
TextureCube prefilterMap5 : TEXTURE : register(t10);
Texture2D depthMapTextures[MAX_NO_OF_LIGHTS] : TEXTURE : register(t11);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);


float Shadows(float4 lightViewPosition, Texture2D depthMapTexture);

float3 fresnelSchlick(float cosTheta, float3 F0);
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float Shadows(float4 viewPos);
float3 pointLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index);
float3 _pointLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index);
float3 sunLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    NR_LIGHTS = lightsNR;
    float4 albedo = float4(pow(objTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz, 2.2f), 1.0f);
    
    //if (albedo.a < 0.1)
    //{
    //    discard;
    //}
    
    //float alphaCheck = objTexture.Sample(SampleTypeWrap, input.inTexCoord).a;
    //if (alphaCheck < 0.1f)
    //{
    //    discard;
    //}
   // float x = -1;
   // clip(x);
    float3 normalColor = normalTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;
    float metallic = metallicTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
    float roughness = roughnessTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
    //roughness = 1.0f;
    float ao = normalTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
    
    float3 ambient = irradianceMap.Sample(SampleTypeWrap, input.inWorldPos.rgb).rgb;
    //float3 ambient = ambientLightColorAndStrength.xyz * (albedo.rgb);

    normalColor = (normalColor * 2.0f) - 1.0f;
    float3 bumpNormal = (normalColor.x * input.inTangent) + (normalColor.y * input.inBinormal) + (normalColor.z * input.inNormal);
    bumpNormal = normalize(bumpNormal);
    float3 V = normalize(cameraPos - input.inWorldPos);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo.rgb, metallic);

    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    //float3 Lo1 = ambient;
    
    [unroll(MAX_NO_OF_LIGHTS)]
    for (int i = 0; i < MAX_NO_OF_LIGHTS; ++i)
    {
        if (i < lightsSize)
        {
            float distance = length(dynamicLightPosition[i].xyz - input.inWorldPos);
            if (distance < Radius1[i].x)
            {
                if (enabledLightsAttribs[i].x == 1)
                {
                    if (enabledLightsAttribs[i].y == 1)
                    {
                        Lo += Shadows(input.lightViewPosition[i], depthMapTextures[i]) * pointLight(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0, i);
                    }
                    else
                    {
                        Lo += pointLight(input, albedo.rgb, bumpNormal, roughness, metallic, V, F0, i);
                    }
          
              
                }
            }
    
        }
        else
        {
            break;
        }
        
   

    }

  
    float3 F = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f), F0, roughness);
    //float3 F = fresnelSchlick(clamp(dot(bumpNormal, V), 0.0f, 1.0f), F0);
    float3 kS = F;
    float3 kD = 1.0f - kS;
    //kD *= 1.0f - metallic;
    float3 irradiance = irradianceMap.Sample(SampleTypeWrap, bumpNormal.rgb).rgb;
    float3 diffuse = irradiance * albedo.rgb;
    
    
    float3 I = normalize(input.inWorldPos - cameraPos);
    float3 R = reflect(I, normalize(bumpNormal));
    
    float3 prefilteredColor = prefilterMap5.Sample(SampleTypeWrap, R) * 0.05f;
    
    if (roughness < 0.5)
    {
        
        if (roughness < 0.5 && roughness > 0.45)
        {
            prefilteredColor = prefilterMap5.Sample(SampleTypeWrap, R);
        }
        if (roughness <= 0.45 && roughness > 0.4)
        {
            prefilteredColor = prefilterMap4.Sample(SampleTypeWrap, R);
        }
        if (roughness <= 0.4 && roughness > 0.3)
        {
            prefilteredColor = prefilterMap3.Sample(SampleTypeWrap, R);
        }
        if (roughness <= 0.3 && roughness > 0.2)
        {
            prefilteredColor = prefilterMap2.Sample(SampleTypeWrap, R);
        }
        if (roughness <= 0.2 && roughness >= 0.0)
        {
            prefilteredColor = prefilterMap1.Sample(SampleTypeWrap, R);
        }
       
    }
   
    float2 brdf = brdfLUT.Sample(SampleTypeWrap, float2(max(dot(bumpNormal, V), 0.0), roughness));
    //float2 brdf = brdfLUT.Sample(SampleTypeClamp, float2(max(dot( input.inNormal,V), 0.0), roughness));

    float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    ambient = (kD * diffuse + specular) * ambientLightColorAndStrength.w;

  
    float3 color = ambient + Lo;
    
 
    //float reflectance = 1 / roughness;
    //float3 refectionMap = (((environmentTexture.Sample(SampleTypeWrap, R)) * roughness * metallic));

    
    float2 projectTexCoord;
    projectTexCoord.x = input.ViewPosition.x / input.ViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.ViewPosition.y / input.ViewPosition.w / 2.0f + 0.5f;
 
    //float3 bloom = bloomTexture.Sample(SampleTypeWrap, projectTexCoord);
   
    //color += bloom;
    
    
    //color += refectionMap * albedo.rgb;
    
 
    color = color / (color + float3(1.0, 1.0f, 1.0f));
    color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    
 
    
    
    //color = (shadow) * color;


    return float4(color, 0.5f);


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
        
       
        int PCF_RANGE = 3;
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
        shadow = 1.0f;
    }
  
    return shadow; //* ambientLightColorAndStrength.w;
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
   // float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
   // float lightDepthValue;

     
    projectTexCoord.x = viewPos.x / viewPos.w / 2.0f + 0.5f;
    projectTexCoord.y = -viewPos.y / viewPos.w / 2.0f + 0.5f;
 

    //depthValue = shadowTexture.Sample(SampleTypeClamp, projectTexCoord).r + ambientLightColorAndStrength.w;
  
    
    
    return depthValue;
   
}

float3 pointLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index)
{
    float3 L = normalize(dynamicLightPosition[index].xyz - input.inWorldPos.xyz).xyz;
    float3 H = normalize(V + L);
        
    float distance = length(dynamicLightPosition[index].xyz - input.inWorldPos.xyz);
    float attenuation = 1.0f / (distance * distance);
    float3 radiance = dynamicLightColor[index].xyz * attenuation;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);


        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f) + 0.001;
    float3 specular = (nominator / denominator) * dynamicLightColor[index].w;
        
    float3 kS = F;
    //float3 kS = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f),F0,roughness);
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}
float3 _pointLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0, int index)
{
    float3 L = normalize(_dynamicLightPosition[index].xyz - input.inWorldPos.xyz).xyz;
    float3 H = normalize(V + L);
        
    float distance = length(_dynamicLightPosition[index].xyz - input.inWorldPos.xyz);
    float attenuation = 1.0f / (distance * distance);
    float3 radiance = _dynamicLightColor[index].xyz * attenuation;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);


        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f) + 0.001;
    float3 specular = (nominator / denominator) * _dynamicLightColor[index].w;
        
    float3 kS = F;
    //float3 kS = fresnelSchlickRoughness(max(dot(bumpNormal, V), 0.0f),F0,roughness);
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 sunLight(PS_INPUT input, float3 albedo, float3 bumpNormal, float roughness, float metallic, float3 V, float3 F0)
{
    float3 lightDir = sunlightDirectionAndSpecularPower.xyz;

    if (sunlightDirectionAndSpecularPower.y < 0)
    {
        lightDir.y = -sunlightDirectionAndSpecularPower.y;

    }
    else if (sunlightDirectionAndSpecularPower.y == 0)
    {
        lightDir.y = 0.01f;

    }
 
    
   // float3 L = normalize(sunLightPosition.xyz - input.inWorldPos.xyz).xyz;
    float3 L = normalize(lightDir.xyz);
    float3 H = normalize(V + L);
        
    
    float3 radiance = sunLightColor.xyz;

        
    float NDF = DistributionGGX(bumpNormal, H, roughness);
    float G = GeometrySmith(bumpNormal, V, L, roughness);
    float3 F = fresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

        
    float3 nominator = NDF * G * F;
    float denominator = 4 * max(dot(bumpNormal, V), 0.0f) * max(dot(bumpNormal, L), 0.0f);
    float3 specular = nominator / max(denominator, 0.001f) * sunlightDirectionAndSpecularPower.w;
        
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
        
    kD *= 1.0f - metallic;
        
    float NdotL = max(dot(bumpNormal, L), 0.0f);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);

}