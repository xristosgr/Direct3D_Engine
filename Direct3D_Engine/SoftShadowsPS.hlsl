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
    
    
    bool EnableLights;
    float3 padding;
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
    float4 sunLightViewPosition : TEXCOORD5;
    float4 lightViewPosition1 : TEXCOORD6;
    float4 lightViewPosition2 : TEXCOORD7;
    
   

};

Texture2D objTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D specularTexture : TEXTURE : register(t2);
Texture2D shadowTexture : TEXTURE : register(t3);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

float3 CalcSunLight(PS_INPUT input, float3 bumpNormal);
float3 CalcSunSpecularLight(PS_INPUT input, float3 bumpNormal);
float3 CalcSpecularLight(PS_INPUT input, float3 bumpNormal, int index);
float3 CalcPointLight(PS_INPUT input, float3 bumpNormal, int index);
float Shadows(float4 viewPos);


static float3 diffuseLightIntensity;

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float4 sampleColor = objTexture.Sample(SampleTypeWrap, input.inTexCoord);
    float3 normalColor = normalTexture.Sample(SampleTypeWrap, input.inTexCoord);
    
    
    float3 result = float3(0.0f, 0.0f, 0.0f);
    normalColor = (normalColor * 2.0f) - 1.0f;
    float3 bumpNormal = (normalColor.x * input.inTangent) + (normalColor.y * input.inBinormal) + (normalColor.z * input.inNormal);
    bumpNormal = normalize(bumpNormal);
    
    float3 ambientLight = ambientLightColorAndStrength.xyz * ambientLightColorAndStrength.w;
    
    if (sampleColor.a < 0.8)
    {
        discard;
    }
    if (enableSunLight)
    {
        result += (CalcSunLight(input, bumpNormal)) + CalcSunSpecularLight(input, bumpNormal);
    }

    

    for (int i = 0; i < NR_LIGHTS; i++)
    {

        
        result += CalcPointLight(input, bumpNormal, i) + CalcSpecularLight(input, bumpNormal, i);
        
    }
    saturate(result);
    result += ambientLight;
    result *= sampleColor;
    

    result *= Shadows(input.ViewPosition);
 
   // result *= sampleColor;
    
   return saturate(float4(result, 1.0f));

}
    

float3 CalcPointLight(PS_INPUT input, float3 bumpNormal, int index)
{

    float3 vectorToLight = dynamicLightPosition[index].xyz - input.inWorldPos;
    diffuseLightIntensity = max(dot(normalize(vectorToLight), bumpNormal), 0);
    float distanceToLight = distance(dynamicLightPosition[index].xyz, input.inWorldPos);
    float attenuationFactor = 1 / (dynamicLightAttenuationAndStrength[index].x + dynamicLightAttenuationAndStrength[index].y * diffuseLightIntensity + dynamicLightAttenuationAndStrength[index].z * pow(distanceToLight, 2));
    diffuseLightIntensity *= attenuationFactor;
    float3 diffuseLight = diffuseLightIntensity * dynamicLightColor[index].w * dynamicLightColor[index].xyz;

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
    
    reflection = diffuseLightIntensity - 2.0f * dot(bumpNormal, diffuseLightIntensity) * bumpNormal;
    float spec =  pow(max(dot(input.playerViewDirSpecular,reflection), 0.0f), 32);
    
    specular = lightDirectionAndSpecularPower[index].w * spec * specularColor[index];

    return saturate(specular * specularIntensity);
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

float3 CalcSunSpecularLight(PS_INPUT input, float3 bumpNormal)
{
    float3 lightDir;
    float3 reflection;
    float3 specularIntensity = specularTexture.Sample(SampleTypeWrap, input.inTexCoord);
    float3 specular;
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
   // lightDir = -lightDirectionAndSpecularPower[index].xyz;
    
    reflection = diffuseLightIntensity - 2.0f * dot(bumpNormal, diffuseLightIntensity) * bumpNormal;
    float spec = pow(max(dot(input.playerViewDirSpecular, reflection), 0.0f), 32);
    
    specular = sunlightDirectionAndSpecularPower.w/10000000000000.0f * spec * sunspecularColor;

    return saturate(specular * specularIntensity);
    
    
    
    
   // float3 reflection;
   // float3 specularIntensity = specularTexture.Sample(SampleTypeWrap, input.inTexCoord);
   // float3 specular;
   // specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
   // reflection =  dot(bumpNormal, diffuseLightIntensity) * bumpNormal;
   // float spec = pow(max(dot(input.playerViewDirSpecular, reflection), 0.0f), 32);
   //
   // specular = 100*sunlightDirectionAndSpecularPower.w * sunspecularColor * spec;
   // 
   //
   // return specular * specularIntensity;
}





float3 CalcSunLight(PS_INPUT input, float3 bumpNormal)
{
    float3 vectorToLight = sunLightPosition.xyz - input.inWorldPos;
    float3 lightDir;

    if (sunlightDirectionAndSpecularPower.y > 0)
    {
        lightDir = sunlightDirectionAndSpecularPower.xyz;
    }
    else if (sunlightDirectionAndSpecularPower.y == 0)
    {
        lightDir.x = 0.0;
        lightDir.y = 0.0001;
        lightDir.z = 0.0;

    }
    else
    {
        lightDir = -sunlightDirectionAndSpecularPower.xyz;
    }

    diffuseLightIntensity = max(dot(normalize(lightDir), bumpNormal), 0) + max(dot(normalize(vectorToLight), bumpNormal), 0);
   // diffuseLightIntensity = max(dot(normalize(vectorToLight), bumpNormal), 0);
   // diffuseLightIntensity = max(dot(normalize(vectorToLight), bumpNormal), 0);
    float3 diffuseLight;
    diffuseLight = diffuseLightIntensity * sunLightColor.w * sunLightColor.xyz;
   
    return diffuseLight;
    
    

 
  
}
