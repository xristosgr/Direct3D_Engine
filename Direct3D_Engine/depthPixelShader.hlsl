//Texture2D shaderTexture1 : TEXTURE : register(t0);

SamplerState SampleType : register(s1);


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float4 depthPosition : TEXTURE4;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
    float depthValue;
    float4 color;

    //float4 TextureColor = shaderTexture1.Sample(SampleType, input.tex);
    //Get the depth value of the ixel by dividing the Z depth by the homogenous W coordinate
    
    //if(TextureColor.a>0.8)
    //{
    //    depthValue = input.depthPosition.z / input.depthPosition.w;
    //}
    //else
    //{
    //    discard;
    //}

    //input.depthPosition.z =  1 / input.depthPosition.z;

    depthValue = input.depthPosition.z / input.depthPosition.w;
    
   
    color = float4(depthValue, depthValue, depthValue, 1.0f);
    
  
    return color;
}
