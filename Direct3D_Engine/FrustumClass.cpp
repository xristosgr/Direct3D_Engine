#include "FrustumClass.h"
#include <algorithm>

FrustumClass::FrustumClass()
{
}


void FrustumClass::ConstructFrustum(float screenDepth,XMMATRIX& view, XMMATRIX& Proj)
{
    float r;
    float zMinimum;
    // Left Frustum Plane
    // Add first column of the matrix to the fourth column
    XMMATRIX viewProjMatrix = DirectX::XMMatrixMultiply(view,Proj);
    
    DirectX::XMStoreFloat4x4(&viewProj, viewProjMatrix);
    XMFLOAT4X4 viewFloat4x4;
    XMFLOAT4X4 projFloat4x4;
    DirectX::XMStoreFloat4x4(&viewFloat4x4, view);
    DirectX::XMStoreFloat4x4(&projFloat4x4, Proj);

    zMinimum = -projFloat4x4._43 / projFloat4x4._33;
    r = screenDepth / (screenDepth - zMinimum);
    projFloat4x4._33 = r;
    projFloat4x4._43 = -r * zMinimum;
    
    Proj = DirectX::XMLoadFloat4x4(&projFloat4x4);
    view = DirectX::XMLoadFloat4x4(&viewFloat4x4);

    viewProjMatrix = DirectX::XMMatrixMultiply(view, Proj);
    DirectX::XMStoreFloat4x4(&viewProj, viewProjMatrix);
    // Calculate near plane of frustum.
     FrustumPlane[0].x = viewProj._14 + viewProj._13;
     FrustumPlane[0].y = viewProj._24 + viewProj._23;
     FrustumPlane[0].z = viewProj._34 + viewProj._33;
     FrustumPlane[0].w = viewProj._44 + viewProj._43;

     m_planes[0] = DirectX::XMLoadFloat4(&FrustumPlane[0]);
     m_planes[0] = DirectX::XMPlaneNormalize(m_planes[0]);


    // Calculate far plane of frustum.
    FrustumPlane[1].x = viewProj._14 - viewProj._13;
    FrustumPlane[1].y = viewProj._24 - viewProj._23;
    FrustumPlane[1].z = viewProj._34 - viewProj._33;
    FrustumPlane[1].w = viewProj._44 - viewProj._43;
    m_planes[1] = DirectX::XMLoadFloat4(&FrustumPlane[1]);
    m_planes[1] = DirectX::XMPlaneNormalize(m_planes[1]);


    // Calculate left plane of frustum.
    FrustumPlane[2].x = viewProj._14 + viewProj._11;
    FrustumPlane[2].y = viewProj._24 + viewProj._21;
    FrustumPlane[2].z = viewProj._34 + viewProj._31;
    FrustumPlane[2].w = viewProj._44 + viewProj._41;
    m_planes[2] = DirectX::XMLoadFloat4(&FrustumPlane[2]);
    m_planes[2] = DirectX::XMPlaneNormalize(m_planes[2]);


    // Calculate right plane of frustum.
    FrustumPlane[3].x = viewProj._14 - viewProj._11;
    FrustumPlane[3].y = viewProj._24 - viewProj._21;
    FrustumPlane[3].z = viewProj._34 - viewProj._31;
    FrustumPlane[3].w = viewProj._44 - viewProj._41;
    m_planes[3] = DirectX::XMLoadFloat4(&FrustumPlane[3]);
    m_planes[3] = DirectX::XMPlaneNormalize(m_planes[3]);


    // Calculate top plane of frustum.
    FrustumPlane[4].x = viewProj._14 - viewProj._12;
    FrustumPlane[4].y = viewProj._24 - viewProj._22;
    FrustumPlane[4].z = viewProj._34 - viewProj._32;
    FrustumPlane[4].w = viewProj._44 - viewProj._42;
    m_planes[4] = DirectX::XMLoadFloat4(&FrustumPlane[4]);
    m_planes[4] = DirectX::XMPlaneNormalize(m_planes[4]);


    // Calculate bottom plane of frustum.
    FrustumPlane[5].x = viewProj._14 + viewProj._12;
    FrustumPlane[5].y = viewProj._24 + viewProj._22;
    FrustumPlane[5].z = viewProj._34 + viewProj._32;
    FrustumPlane[5].w = viewProj._44 + viewProj._42;
    m_planes[5] = DirectX::XMLoadFloat4(&FrustumPlane[5]);
    m_planes[5] = DirectX::XMPlaneNormalize(m_planes[5]);

}

bool FrustumClass::CheckRect(float& xCenter, float& yCenter, float& zCenter, float xSize, float ySize, float zSize)
{
    for (int i = 0; i < 6; i++)
    {
        if(XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter - xSize), (yCenter - ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter + xSize), (yCenter - ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter - xSize), (yCenter + ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter - xSize), (yCenter - ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter + xSize), (yCenter + ySize), (zCenter - zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter + xSize), (yCenter - ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter - xSize), (yCenter + ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVECTOR{ (xCenter + xSize), (yCenter + ySize), (zCenter + zSize) })) >= 0)
        {
            continue;
        }
        return false;
    }
    return true;
}









TerrainFrustumClass::TerrainFrustumClass()
{

}

void TerrainFrustumClass::Initialize(float screenDepth)
{
    m_screenDepth = screenDepth;
}

void TerrainFrustumClass::ConstructFrustum(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
    XMFLOAT4X4 pMatrix, matrix;
    float zMinimum, r, length;
    XMMATRIX finalMatrix;


    // Convert the projection matrix into a 4x4 float type.
    XMStoreFloat4x4(&pMatrix, projectionMatrix);

    // Calculate the minimum Z distance in the frustum.
    zMinimum = -pMatrix._43 / pMatrix._33;
    r = m_screenDepth / (m_screenDepth - zMinimum);

    // Load the updated values back into the projection matrix.
    pMatrix._33 = r;
    pMatrix._43 = -r * zMinimum;
    projectionMatrix = XMLoadFloat4x4(&pMatrix);

    // Create the frustum matrix from the view matrix and updated projection matrix.
    finalMatrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

    // Convert the final matrix into a 4x4 float type.
    XMStoreFloat4x4(&matrix, finalMatrix);

    // Calculate near plane of frustum.
    m_planes[0][0] = matrix._14 + matrix._13;
    m_planes[0][1] = matrix._24 + matrix._23;
    m_planes[0][2] = matrix._34 + matrix._33;
    m_planes[0][3] = matrix._44 + matrix._43;

    // Normalize the near plane.
    length = sqrtf((m_planes[0][0] * m_planes[0][0]) + (m_planes[0][1] * m_planes[0][1]) + (m_planes[0][2] * m_planes[0][2]));
    m_planes[0][0] /= length;
    m_planes[0][1] /= length;
    m_planes[0][2] /= length;
    m_planes[0][3] /= length;

    // Calculate far plane of frustum.
    m_planes[1][0] = matrix._14 - matrix._13;
    m_planes[1][1] = matrix._24 - matrix._23;
    m_planes[1][2] = matrix._34 - matrix._33;
    m_planes[1][3] = matrix._44 - matrix._43;

    // Normalize the far plane.
    length = sqrtf((m_planes[1][0] * m_planes[1][0]) + (m_planes[1][1] * m_planes[1][1]) + (m_planes[1][2] * m_planes[1][2]));
    m_planes[1][0] /= length;
    m_planes[1][1] /= length;
    m_planes[1][2] /= length;
    m_planes[1][3] /= length;

    // Calculate left plane of frustum.
    m_planes[2][0] = matrix._14 + matrix._11;
    m_planes[2][1] = matrix._24 + matrix._21;
    m_planes[2][2] = matrix._34 + matrix._31;
    m_planes[2][3] = matrix._44 + matrix._41;

    // Normalize the left plane.
    length = sqrtf((m_planes[2][0] * m_planes[2][0]) + (m_planes[2][1] * m_planes[2][1]) + (m_planes[2][2] * m_planes[2][2]));
    m_planes[2][0] /= length;
    m_planes[2][1] /= length;
    m_planes[2][2] /= length;
    m_planes[2][3] /= length;

    // Calculate right plane of frustum.
    m_planes[3][0] = matrix._14 - matrix._11;
    m_planes[3][1] = matrix._24 - matrix._21;
    m_planes[3][2] = matrix._34 - matrix._31;
    m_planes[3][3] = matrix._44 - matrix._41;

    // Normalize the right plane.
    length = sqrtf((m_planes[3][0] * m_planes[3][0]) + (m_planes[3][1] * m_planes[3][1]) + (m_planes[3][2] * m_planes[3][2]));
    m_planes[3][0] /= length;
    m_planes[3][1] /= length;
    m_planes[3][2] /= length;
    m_planes[3][3] /= length;

    // Calculate top plane of frustum.
    m_planes[4][0] = matrix._14 - matrix._12;
    m_planes[4][1] = matrix._24 - matrix._22;
    m_planes[4][2] = matrix._34 - matrix._32;
    m_planes[4][3] = matrix._44 - matrix._42;

    // Normalize the top plane.
    length = sqrtf((m_planes[4][0] * m_planes[4][0]) + (m_planes[4][1] * m_planes[4][1]) + (m_planes[4][2] * m_planes[4][2]));
    m_planes[4][0] /= length;
    m_planes[4][1] /= length;
    m_planes[4][2] /= length;
    m_planes[4][3] /= length;

    // Calculate bottom plane of frustum.
    m_planes[5][0] = matrix._14 + matrix._12;
    m_planes[5][1] = matrix._24 + matrix._22;
    m_planes[5][2] = matrix._34 + matrix._32;
    m_planes[5][3] = matrix._44 + matrix._42;

    // Normalize the bottom plane.
    length = sqrtf((m_planes[5][0] * m_planes[5][0]) + (m_planes[5][1] * m_planes[5][1]) + (m_planes[5][2] * m_planes[5][2]));
    m_planes[5][0] /= length;
    m_planes[5][1] /= length;
    m_planes[5][2] /= length;
    m_planes[5][3] /= length;
}

bool TerrainFrustumClass::CheckRectangle(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth)
{
    int i;
    float dotProduct;


    // Check if any of the 6 planes of the rectangle are inside the view frustum.
    for (i = 0; i < 6; i++)
    {
        dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * minDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * minHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * minWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        dotProduct = ((m_planes[i][0] * maxWidth) + (m_planes[i][1] * maxHeight) + (m_planes[i][2] * maxDepth) + (m_planes[i][3] * 1.0f));
        if (dotProduct >= 0.0f)
        {
            continue;
        }

        return false;
    }

    return true;
}
