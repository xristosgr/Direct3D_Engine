#define NR_LIGHTS 2
#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

    float4 cameraPos;

};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(float4(input.position, 1.0f), wvpMatrix);
    output.tex = input.tex;




    return output;
}