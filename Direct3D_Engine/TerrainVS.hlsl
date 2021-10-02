

#include "ShaderSettings.hlsli"
#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0)
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
 
    
    //float4 cameraPos;
    


};

cbuffer cameraBuffer : register(b3)
{
    float4 cameraPos;

};


struct VS_INPUT
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD0;
    float3 inNormal : NORMAL;
 
    float3 inTangent : TANGENT;
    float3 inBinormal : BINORMAL;
    float3 color : COLOR;
    float2 inTexCoord2 : TEXCOORD1;


};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD0;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WOLRD_POSITION;
   
    float3 outTangent : TANGENT;
    float3 outBinormal : BINORMAL;
    //float3 color : COLOR;
    //float3 color : COLOR;
    float4 ViewPosition : TEXCOORD1;


    float3 playerViewDir : TEXCOORD2;
    float3 playerViewDirSpecular : TEXCOORD3;
    
    float4 depthPosition : TEXTURE4;
    //float4 sunLightViewPosition : TEXCOORD5;
    //float4 lightViewPosition1 : TEXCOORD6;
    //float4 lightViewPosition2 : TEXCOORD7;
    
    
    float3 TangentLightPos : TEXCOORD5;
    float3 TangentViewPos : TEXCOORD6;
    float3 TangentFragPos : TEXCOORD7;
    
    float3 color : COLOR;
    float2 outTexCoord2 : TEXCOORD8;

};

VS_OUTPUT main(VS_INPUT input)
{
    
        
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

    //output.color = float4(input.color, 1.0f);
 
    output.ViewPosition = output.outPosition;
    //output.ViewPosition = float4(cameraPos.xyz, 1.0f);
    
    output.playerViewDirSpecular = cameraPos.xyz - output.outWorldPos.xyz;
    output.playerViewDirSpecular = normalize(output.playerViewDirSpecular);
    
    
    float3 T = normalize(mul(float4(input.inTangent, 1.0f), worldMatrix));
    float3 B = normalize(mul(float4(input.inBinormal, 1.0f), worldMatrix));
    float3 N = normalize(mul(float4(input.inNormal, 1.0f), worldMatrix));
    float3x3 TBN = transpose(float3x3(output.outTangent, output.outBinormal, output.outNormal));
    
    float3 lightPos = float3(0, 10, 10);
    //output.TangentLightPos = mul(sunPos.xyz, TBN);
    output.TangentViewPos = mul(cameraPos.xyz - output.outPosition.xyz, TBN);
    output.TangentFragPos = mul(output.outPosition.xyz,TBN);
    
    output.depthPosition = output.outPosition;
    
    //input.inTexCoord2.y += 1000;
    output.outTexCoord2 = input.inTexCoord2;
    output.color = input.color;
    
    return output;
}