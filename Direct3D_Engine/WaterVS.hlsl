

#include "ShaderSettings.hlsli"
#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;


    
    
    


};

cbuffer waterBuffer : register(b3)
{
    float4 cameraPos;
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
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WOLRD_POSITION;
    float3 outTangent : TANGENT;
    float3 outBinormal : BINORMAL;
    
    float4 ViewPosition : TEXCOORD1;


    float3 playerViewDir : TEXCOORD2;
    float3 playerViewDirSpecular : TEXCOORD3;
    
    float4 depthPosition : TEXTURE4;
    
    float3 outTangentLightPos : TEXCOORD5;
    float3 outTangentViewPos : TEXCOORD6;
    float3 outTangentFragPos : TEXCOORD7;
    
    float heightData : TEXCOORD8;
    
    
   
};


VS_OUTPUT main(VS_INPUT input)
{

   // input.inNormal.xz = time.x;
  
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), worldMatrix);
    
    output.heightData = input.inPos.y;
    

    output.outPosition = mul(output.outPosition, viewMatrix);
    output.outPosition = mul(output.outPosition, projectionMatrix);
    output.outTexCoord = input.inTexCoord;
    
   
    //Calculate normals
        output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMatrix)); //set normalMatrix's w to 0 to omit translation with worldMatrix
    output.outTangent = normalize(mul(float4(input.inTangent, 0.0f), worldMatrix));
    output.outBinormal = normalize(mul(float4(input.inBinormal, 0.0f), worldMatrix));
    
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMatrix);

    float3x3 TBN = transpose(float3x3(output.outTangent, output.outBinormal, output.outNormal));
    output.outTangentLightPos = mul(cameraPos.xyz, TBN);
    output.outTangentViewPos = mul(cameraPos.xyz, TBN);
    output.outTangentFragPos = mul(output.outPosition.xyz, TBN);
    
    output.ViewPosition = output.outPosition;
    //output.ViewPosition = float4(cameraPos.xyz, 1.0f);
    
    output.playerViewDirSpecular = cameraPos.xyz - output.outWorldPos.xyz;
    output.playerViewDirSpecular = normalize(output.playerViewDirSpecular);
    
    
  
    
    return output;
}