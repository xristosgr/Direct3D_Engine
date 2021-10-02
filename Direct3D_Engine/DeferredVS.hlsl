#include "ShaderSettings.hlsli"
#pragma pack_matrix(row_major)


cbuffer perObjectBuffer : register(b0)
{
 
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;

  

    float4x4 bones[100];
};
cbuffer cameraBuffer : register(b3)
{
    float4 cameraPos;

};

cbuffer lightsBuffer : register(b4)
{
    float4x4 lightViewMatrix[2][2];
    float4x4 lightProjectionMatrix[2][2];
    
    
    //float4x4 cascadedViewMatrix[4];
    //float4x4 cascadedProjectionMatrix[4];

};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
 

};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD0;
    //float3 outWorldPos : WOLRD_POSITION;
    float4 ViewPosition : TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT input)
{
  
    VS_OUTPUT output;
  
    output.outPosition = mul(float4(input.inPos.xyz, 1.0f), wvpMatrix);

    output.outTexCoord = input.inTexCoord;


    output.ViewPosition = output.outPosition;
   // output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

    return output;
}