#define NR_LIGHTS 1
cbuffer lightBuffer : register(b0)
{
    float3 dynamicLightPosition[NR_LIGHTS];
    float4 ambientLightColorAndStrength;
    float4 dynamicLightAttenuationAndStrength[NR_LIGHTS];
    float3 dynamicLightColor[NR_LIGHTS];
    
    float4 lightDirectionAndSpecularPower[NR_LIGHTS];
    float4 specularColor[NR_LIGHTS];
}


struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    
    float4 lightViewPosition1 : TEXCOORD1;
    //float4 lightViewPosition2 : TEXCOORD2;
    
    float3 playerViewDir : TEXCOORD3;

    
    
};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D specularTexture : TEXTURE : register(t2);
Texture2D depthMapTexture1: TEXTURE : register(t3);
//Texture2D depthMapTexture2 : TEXTURE : register(t4);


SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

float3 CalcSpecularLight(PS_INPUT input, float3 bumpNormal,int index);
float3 CalcPointLight(PS_INPUT input, float3 bumpNormal, int index);
float3 Shadows(PS_INPUT input, Texture2D depthMap, float4 lightViewPosition, float bias, float2 projectTexCoord, float depthValue, float lightDepthValue, float3 ambientLight, float3 appliedLight, float3 bumpNormal,int index);

static float3 diffuseLightIntensity;

float4 main(PS_INPUT input) : SV_TARGET
{
    float bias = 0.001f;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    
  

    
    float3 sampleColor = objTexture.Sample(SampleTypeWrap, input.inTexCoord);
    float3 normalColor = normalTexture.Sample(SampleTypeWrap, input.inTexCoord);
    
    normalColor = (normalColor * 2.0f) - 1.0f;
    float3 bumpNormal = (normalColor.x * input.inTangent) + (normalColor.y * input.inBinormal) + (normalColor.z * input.inNormal);
    bumpNormal = normalize(bumpNormal);
    
    float3 ambientLight = ambientLightColorAndStrength.xyz * ambientLightColorAndStrength.w;
    float3 appliedLight = ambientLight;
  
    float3 result;
    result = Shadows(input, depthMapTexture1, input.lightViewPosition1, bias, projectTexCoord, depthValue, lightDepthValue, ambientLight, appliedLight, bumpNormal,0);
   // result += Shadows(input, depthMapTexture2, input.lightViewPosition2, bias, projectTexCoord, depthValue, lightDepthValue, ambientLight, appliedLight, bumpNormal,1);

   
    
    return float4(saturate(result * sampleColor), 1.0f);  
}
    

float3 CalcPointLight(PS_INPUT input, float3 bumpNormal, int index)
{

    float3 vectorToLight = dynamicLightPosition[index] - input.inWorldPos;
    diffuseLightIntensity = max(dot(normalize(vectorToLight), bumpNormal), 0);
    float distanceToLight = distance(dynamicLightPosition[index], input.inWorldPos);
    float attenuationFactor = 1 / (dynamicLightAttenuationAndStrength[index].x + dynamicLightAttenuationAndStrength[index].y * diffuseLightIntensity + dynamicLightAttenuationAndStrength[index].z * pow(distanceToLight, 2));
    diffuseLightIntensity *= attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * dynamicLightAttenuationAndStrength[index].w * dynamicLightColor[index];

    return diffuseLight;
 
  
}

float3 CalcSpecularLight(PS_INPUT input, float3 bumpNormal, int index)
{
    float3 lightDir;
    float3 reflection;
    float3 specularIntensity = specularTexture.Sample(SampleTypeWrap, input.inTexCoord);
    float3 specular;
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    lightDir = -lightDirectionAndSpecularPower[index].xyz;
    
    //reflection = 2.0f * diffuseLightIntensity * bumpNormal - lightDir;
    reflection = diffuseLightIntensity - 2.0f * dot(bumpNormal, diffuseLightIntensity) * bumpNormal;
   // reflection = normalize(2 * diffuseLightIntensity * input.inNormal - lightDir);
    specular = specularColor[index] * pow(max(dot(reflection, input.playerViewDir), 0.0f), 32);
    

    return specular * specularIntensity;
}

float3 Shadows(PS_INPUT input, Texture2D depthMap, float4 lightViewPosition, float bias, float2 projectTexCoord, float depthValue, float lightDepthValue, float3 ambientLight, float3 appliedLight,float3 bumpNormal,int index)
{
    projectTexCoord.x = lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;

    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {

        depthValue = depthMap.Sample(SampleTypeClamp, projectTexCoord).r;


        
        
        lightDepthValue = lightViewPosition.z / lightViewPosition.w;
        lightDepthValue = lightDepthValue - bias;
     
        if (lightDepthValue < depthValue)
        {

            float3 shadowLights = CalcPointLight(input, bumpNormal, index);
            appliedLight += (ambientLight + shadowLights + CalcSpecularLight(input, bumpNormal, index));
            
        }
        else
        {
            appliedLight += ambientLight;
        }
    
    }
    else
    {
        appliedLight += ambientLight;
    }
    return appliedLight;
   
}