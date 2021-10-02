#include "ShaderSettings.hlsli"

#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

};
cbuffer shadowParamsBuffer : register(b1)
{
    float screenSize;
    float3 padding;
};
cbuffer blurWeights : register(b2)
{
    float4 weight1234;
    float4 weight5678;
    float weight9;
    float3 padding1;
    bool test;
}
struct VS_INPUT
{
    float3 position : POSITION;
    float2 tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float2 centerTexCoords : TEXCOORD1;
    float2 texCoords[21] : TEXCOORD2;

    
};

VS_OUTPUT main(VS_INPUT input)
{

    
    VS_OUTPUT output;
    
    float texelSize;
    output.position = mul(float4(input.position, 1.0f), wvpMatrix);
    output.tex = input.tex;
    output.centerTexCoords = output.position * 0.5 + 0.5;
    texelSize = 1.0f / window_width;

    for (int i = -10; i <= 10;++i)
    {
        output.texCoords[i + 10] = output.centerTexCoords + float2(texelSize * i*1.5, 0);

    }


        return output;
}