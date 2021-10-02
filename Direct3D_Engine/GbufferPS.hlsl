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

struct PixelOutputType
{
    float4 albedoRough : SV_Target0;
    float4 normalMetallic : SV_Target1;
    float4 Pos : SV_Target2;
};

Texture2D albedoTexture : TEXTURE : register(t0);
Texture2D normalTexture : TEXTURE : register(t1);
Texture2D roughnessTexture : TEXTURE : register(t2);
Texture2D metallicTexture : TEXTURE : register(t3);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);



PixelOutputType main(PS_INPUT input) : SV_TARGET
{
    PixelOutputType output;
    
    output.Pos.rgb = input.inWorldPos;
    output.Pos.a = 1.0f;
    
    output.albedoRough.rgb = albedoTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;


    output.albedoRough.a = roughnessTexture.Sample(SampleTypeWrap, input.inTexCoord).r;

    float3 normal = normalTexture.Sample(SampleTypeWrap, input.inTexCoord).xyz;
    output.normalMetallic.a = metallicTexture.Sample(SampleTypeWrap, input.inTexCoord).r;
    normal = (normal * 2.0f) - 1.0f;
    float3 bumpNormal = (normal.r * input.inTangent) + (normal.g * input.inBinormal) + (normal.b * input.inNormal);
    bumpNormal = normalize(bumpNormal);
    
    output.normalMetallic.rgb = bumpNormal;

    return output;

}

