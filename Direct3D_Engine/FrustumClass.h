#pragma once
#include <d3d11.h>
#include<DirectXMath.h>
#include<SimpleMath.h>
#include<string>
#include<vector>

using namespace DirectX;

class FrustumClass
{
public:
	FrustumClass();

	void ConstructFrustum(float screenDepth,XMMATRIX& view, XMMATRIX& Proj);
	bool CheckRect(float& xCenter, float& yCenter, float& zCenter, float xSize, float ySize, float zSize);
	XMFLOAT4 FrustumPlane[6];


private:
	
	XMFLOAT4X4 viewProj;
	std::vector<XMFLOAT3> modelAABB;

	XMVECTOR m_planes[6];
};


class TerrainFrustumClass
{
public:
	TerrainFrustumClass();

	void Initialize(float);

	void ConstructFrustum(XMMATRIX&, XMMATRIX&);
	bool CheckRectangle(float, float, float, float, float, float);

private:

	float m_screenDepth;
	float m_planes[6][4];

};
