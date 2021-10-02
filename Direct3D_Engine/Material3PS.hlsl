


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



SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

Texture2D roughnessTexture : TEXTURE : register(t2);

float4 main(PS_INPUT input) : SV_TARGET
{
    //float roughness = (roughnessTexture.Sample(SampleTypeWrap, input.inTexCoord)).r;

    return float4(input.inWorldPos, 1.0f);

}

