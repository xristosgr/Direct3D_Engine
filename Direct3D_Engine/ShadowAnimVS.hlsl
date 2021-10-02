

#include "ShaderSettings.hlsli"
//#pragma pack_matrix(row_major)

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
    float4x4 lightViewMatrix[MAX_NO_OF_LIGHTS];
    float4x4 lightProjectionMatrix[MAX_NO_OF_LIGHTS];
    
    
    //float4x4 cascadedViewMatrix[4];
    //float4x4 cascadedProjectionMatrix[4];

};
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
   //float2 outTexCoord : TEXCOORD;
   //float3 outNormal : NORMAL;
   //float3 outWorldPos : WOLRD_POSITION;
   //float3 outTangent : TANGENT;
   //float3 outBinormal : BINORMAL;


    float4 lightViewPosition[2] : LIGHTVIEWS;
};

VS_OUTPUT main(VS_INPUT input)
{

  
    VS_OUTPUT output;
    float Weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    Weights[0] = input.weights.x;
    Weights[1] = input.weights.y;
    Weights[2] = input.weights.z;
    Weights[3] = input.weights.w;

    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 n = float3(0.0f, 0.0f, 0.0f);
    float3 t = float3(0.0f, 0.0f, 0.0f);
    float3 b = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; i++)
    {
        posL += Weights[i] * mul(float4(input.inPos, 1.0f), bones[input.boneIDs[i]]).xyz;
        n += Weights[i] * mul(float4(input.inNormal, 0.0f), bones[input.boneIDs[i]]).xyz;
        t += Weights[i] * mul(float4(input.inTangent, 0.0f), bones[input.boneIDs[i]]).xyz;
        b += Weights[i] * mul(float4(input.inBinormal, 0.0f), bones[input.boneIDs[i]]).xyz;

    }
    
 // float4 p = input.weights[0] * mul(float4(input.inPos, 1.0f), bones[input.boneIDs[0]]);
 // p += input.weights[1] * mul(float4(input.inPos, 1.0f), bones[input.boneIDs[1]]);
 // p += input.weights[2] * mul(float4(input.inPos, 1.0f), bones[input.boneIDs[2]]);
 // p += input.weights[3] * mul(float4(input.inPos, 1.0f), bones[input.boneIDs[3]]);
 // p.w = 1.0f;
    
    output.outPosition = mul(float4(posL, 1.0f), wvpMatrix);
   //
   // output.outPosition = mul(output.outPosition, viewMatrix);
   // output.outPosition = mul(output.outPosition, projectionMatrix);
   // output.outWorldPos = mul(float4(posL, 1.0f), worldMatrix);

 
    
    //output.outTexCoord = input.inTexCoord;


    //Calculate normals
   // output.outNormal = normalize(mul(float4(input.inNormal, 0.0f) * float4(posL, 0.0f), worldMatrix)); //set normalMatrix's w to 0 to omit translation with worldMatrix
   // output.outTangent = normalize(mul(float4(input.inTangent, 0.0f) * float4(posL, 0.0f), worldMatrix));
   // output.outBinormal = normalize(mul(float4(input.inBinormal, 0.0f) * float4(posL, 0.0f), worldMatrix));
   // 
   
        //Calculate normals
    //output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix)); //set normalMatrix's w to 0 to omit translation with worldMatrix
    //output.outTangent = normalize(mul(float4(input.inTangent, 0.0f), worldMatrix));
    //output.outBinormal = normalize(mul(float4(input.inBinormal, 0.0f), worldMatrix));
    //
    //output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

   //output.ViewPosition = output.outPosition;
   //
   //output.playerViewDirSpecular = cameraPos.xyz - output.outWorldPos.xyz;
   //output.playerViewDirSpecular = normalize(output.playerViewDirSpecular);
   //
   //
   //output.depthPosition = output.outPosition;
    
    
    
       
   // for (int i = 0; i < 4; ++i)
   // {
   //     output.cascadeViewPosition[i] = mul(float4(input.inPos, 1.0f), worldMatrix);
   //     output.cascadeViewPosition[i] = mul(output.cascadeViewPosition[i], cascadedViewMatrix[i]);
   //     output.cascadeViewPosition[i] = mul(output.cascadeViewPosition[i], cascadedProjectionMatrix[i]);
   // }
    
    
    
            
    for (int j = 0; j < 2; j++)
    {

            output.lightViewPosition[j] = mul(float4(input.inPos, 1.0f), worldMatrix);
            output.lightViewPosition[j] = mul(output.lightViewPosition[j], lightViewMatrix[j]);
            output.lightViewPosition[j] = mul(output.lightViewPosition[j], lightProjectionMatrix[j]);
   
        
    }
     
   
     
    return output;
}