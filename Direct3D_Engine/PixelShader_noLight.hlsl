struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WOLRD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);

SamplerState objSamplerStateClamp: SAMPLER : register(s0);
SamplerState objSamplerState : SAMPLER : register(s1);



float4 main(PS_INPUT input) : SV_TARGET
{
    float4 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);

   // sampleColor.rgb = sampleColor.rgb / (sampleColor.rgb + float3(1.0, 1.0f, 1.0f));
   // sampleColor.rgb = pow(sampleColor.rgb, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
    
    //if (sampleColor.r == 0 && sampleColor.g == 0 && sampleColor.b == 0)
    //{
    //    return float4(sampleColor.xyz, 0.2f);
    //}
    return float4(sampleColor.xyz, 1.0f);
}