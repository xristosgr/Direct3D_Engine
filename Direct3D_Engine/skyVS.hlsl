#define NR_LIGHTS 1
#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

    

};

struct VS_INPUT
{
    float3 inPos : POSITION;


};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float4 domePosition : TEXCOORD0;
    float4 ViewPosition : TEXCOORD1;


    
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
   
    output.outPosition = mul(float4(input.inPos, 1.0f), wvpMatrix);
    output.domePosition = float4(input.inPos, 1.0f);

    output.ViewPosition = output.outPosition;
    
    return output;
}