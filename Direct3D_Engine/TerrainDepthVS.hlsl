#pragma pack_matrix(row_major)
cbuffer PerFrameBuffer
{
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float4 depthPosition : TEXTURE4;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    

    output.position = mul(output.position, viewMatrix);
    output.depthPosition = output.position;
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    // Store the position value in a second input value for depth value calculations.
   
    
    return output;
}