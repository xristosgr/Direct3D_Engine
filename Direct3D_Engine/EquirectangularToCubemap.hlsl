struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);

SamplerState objSamplerStateClamp : SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);

const float2 invAtan = float2(0.1591, 0.3183);

float2 SampleSphericalMap(float3 v)
{
    float aTan = atan2(v.z, v.x) / 50;
    float2 uv = float2(aTan, asin(v.y) / 16);
    
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
float4 main(PS_INPUT input) : SV_TARGET
{

    float2 uv = SampleSphericalMap(normalize(input.inWorldPos));
    
    float3 sampleColor = objTexture.Sample(objSamplerStateClamp, uv).rgb;

    //sampleColor = sampleColor / (sampleColor + float3(1.0, 1.0f, 1.0f));
    //sampleColor = pow(sampleColor, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    return saturate(float4(sampleColor, 1.0f));
}