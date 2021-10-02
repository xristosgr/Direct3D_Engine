struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
};
cbuffer screenEffectBuffer : register(b9)
{
    float gamma;
    float bloomBrightness;
    float bloomStrength;
}

Texture2D objTexture : TEXTURE : register(t0);
Texture2D bloomTexture : TEXTURE : register(t1);
Texture2D lightsTexture : TEXTURE : register(t2);

SamplerState objSamplerStateClamp : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);



float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    float3 bloom = bloomTexture.Sample(objSamplerState, input.inTexCoord);
    float3 lights = lightsTexture.Sample(objSamplerState, input.inTexCoord);

    sampleColor += bloom * bloomStrength;
    //sampleColor += lights;
    
    return float4(sampleColor.xyz, 1.0f);
}