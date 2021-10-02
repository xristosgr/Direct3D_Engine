
#ifndef BLOCK_SIZE
#pragma message( "BLOCK_SIZE undefined. Default to 16.")
#define BLOCK_SIZE 16 // should be defined by the application.
#endif

struct ComputeShaderInput
{
    uint3 groupID : SV_GroupID; // 3D index of the thread group in the dispatch.
    uint3 groupThreadID : SV_GroupThreadID; // 3D index of local thread ID in a thread group.
    uint3 dispatchThreadID : SV_DispatchThreadID; // 3D index of global thread ID in the dispatch.
    uint groupIndex : SV_GroupIndex; // Flattened local index of the thread within a thread group.
};

cbuffer DispatchParams : register(b4)
{
    // Number of groups dispatched. (This parameter is not available as an HLSL system value!)
    uint3 numThreadGroups;
    // uint padding // implicit padding to 16 bytes.
 
    // Total number of threads dispatched. (Also not available as an HLSL system value!)
    // Note: This value may be less than the actual number of threads executed 
    // if the screen size is not evenly divisible by the block size.
    uint3 numThreads;
    // uint padding // implicit padding to 16 bytes.
}
cbuffer ScreenToViewParams : register(b5)
{
    float4x4 InverseProjection;
    float2 ScreenDimensions;
}

struct Plane
{
    float3 N; // Plane normal.
    float d; // Distance to origin.
};


// Convert clip space coordinates to view space
float4 ClipToView(float4 clip)
{
    // View space position.
    float4 view = mul(InverseProjection, clip);
    // Perspective projection.
    view = view / view.w;
 
    return view;
}
 
// Convert screen space coordinates to view space.
float4 ScreenToView(float4 screen)
{
    // Convert to normalized texture coordinates
    float2 texCoord = screen.xy / ScreenDimensions;
 
    // Convert to clip space
    float4 clip = float4(float2(texCoord.x, 1.0f - texCoord.y) * 2.0f - 1.0f, screen.z, screen.w);
 
    return ClipToView(clip);
}

Plane ComputePlane(float3 p0, float3 p1, float3 p2)
{
    Plane plane;
 
    float3 v0 = p1 - p0;
    float3 v2 = p2 - p0;
 
    plane.N = normalize(cross(v0, v2));
 
    // Compute the distance to the origin using p0.
    plane.d = dot(plane.N, p0);
 
    return plane;
}
struct Frustum
{
    Plane planes[4]; // left, right, top, bottom frustum planes.
};

Texture2D<float> InputTex : register(t0);
RWStructuredBuffer<Frustum> out_Frustums : register(u0);

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{
    // View space eye position is always at the origin.
    const float3 eyePos = float3(0, 0, 0);
 
    // Compute the 4 corner points on the far clipping plane to use as the 
    // frustum vertices.
    float4 screenSpace[4];
    // Top left point
    screenSpace[0] = float4(IN.dispatchThreadID.xy * BLOCK_SIZE, -1.0f, 1.0f);
    // Top right point
    screenSpace[1] = float4(float2(IN.dispatchThreadID.x + 1, IN.dispatchThreadID.y) * BLOCK_SIZE, -1.0f, 1.0f);
    // Bottom left point
    screenSpace[2] = float4(float2(IN.dispatchThreadID.x, IN.dispatchThreadID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f);
    // Bottom right point
    screenSpace[3] = float4(float2(IN.dispatchThreadID.x + 1, IN.dispatchThreadID.y + 1) * BLOCK_SIZE, -1.0f, 1.0f);
    
    float3 viewSpace[4];
// Now convert the screen space points to view space
    for (int i = 0; i < 4; i++)
    {
        viewSpace[i] = ScreenToView(screenSpace[i]).xyz;
    }
    
    // Now build the frustum planes from the view space points
    Frustum frustum;
 
// Left plane
    frustum.planes[0] = ComputePlane(eyePos, viewSpace[2], viewSpace[0]);
// Right plane
    frustum.planes[1] = ComputePlane(eyePos, viewSpace[1], viewSpace[3]);
// Top plane
    frustum.planes[2] = ComputePlane(eyePos, viewSpace[0], viewSpace[1]);
// Bottom plane
    frustum.planes[3] = ComputePlane(eyePos, viewSpace[3], viewSpace[2]);
    // Store the computed frustum in global memory (if our thread ID is in bounds of the grid).
    if (IN.dispatchThreadID.x < numThreads.x && IN.dispatchThreadID.y < numThreads.y)
    {
        uint index = IN.dispatchThreadID.x + (IN.dispatchThreadID.y * numThreads.x);
        out_Frustums[index] = frustum;
    }
}