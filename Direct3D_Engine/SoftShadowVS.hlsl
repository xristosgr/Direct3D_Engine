

#include "ShaderSettings.hlsli"
#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;


    
    


};
cbuffer cameraBuffer : register(b3)
{
    float4 cameraPos;

};

cbuffer lightsBuffer : register(b4)
{
    float4x4 lightViewMatrix[MAX_NO_OF_LIGHTS];
    float4x4 lightProjectionMatrix[MAX_NO_OF_LIGHTS];
    
    
    //float4x4 cascadedViewMatrix[4];
    //float4x4 cascadedProjectionMatrix[4];

};

cbuffer lightGlobalBuffer : register(b5)
{
    unsigned int lightsSize;
   // unsigned int shadowLightSize;
}


struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    
    
    float4 weights : WEIGHTS;
    uint4 boneIDs : BONE_IDs;

};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WOLRD_POSITION;
    float3 outTangent : TANGENT;
    float3 outBinormal : BINORMAL;
    
    float4 ViewPosition : TEXCOORD1;
    
    float4 lightViewPosition[MAX_NO_OF_SHADOWS] : LIGHTVIEWS;
   
};

VS_OUTPUT main(VS_INPUT input)
{
   // if (!isAnimated)
   // {
   //     #pragma pack_matrix(row_major);
   // }
        
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), worldMatrix);
    
   
       
    

    output.outPosition = mul(output.outPosition, viewMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);
    output.outTexCoord = input.inTexCoord;

    //Calculate normals
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix)); //set normalMatrix's w to 0 to omit translation with worldMatrix
    output.outTangent = normalize(mul(float4(input.inTangent, 0.0f), worldMatrix));
    output.outBinormal = normalize(mul(float4(input.inBinormal, 0.0f), worldMatrix));
    
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);


   output.ViewPosition = output.outPosition;
    
    //output.ViewPosition = float4(cameraPos.xyz, 1.0f);
    
    //output.playerViewDirSpecular = cameraPos.xyz - output.outWorldPos.xyz;
    //output.playerViewDirSpecular = normalize(output.playerViewDirSpecular);
    
    for (int j = 0; j < lightsSize; j++)
    {

        output.lightViewPosition[j] = mul(float4(input.inPos, 1.0f), worldMatrix);
        output.lightViewPosition[j] = mul(output.lightViewPosition[j], lightViewMatrix[j]);
        output.lightViewPosition[j] = mul(output.lightViewPosition[j], lightProjectionMatrix[j]);
   
     
    }
    
    return output;
}