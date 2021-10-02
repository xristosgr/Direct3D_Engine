#pragma once
#include <DirectXMath.h>

struct Vertex
{
	Vertex() {
		
	}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny,float nz,float tX,float tY,float tZ,float bX,float bY, float bZ)
		: pos(x, y, z), texCoord(u, v), normal(nx,ny,nz), tangent(tX,tY,tZ),binormal(bX,bY,bZ) 
	
	{

	}
	Vertex(float x, float y, float z, float u, float v, float nx, float ny, float nz, float tX, float tY, float tZ, float bX, float bY, float bZ,float r,float g,float b)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz), tangent(tX, tY, tZ), binormal(bX, bY, bZ),color(r,g,b)
	{

	}
	Vertex(float x, float y, float u, float v)
		:pos(x, y, 0.0f), texCoord(u, v)
	{
		
	}
	Vertex(float x, float y,float z,  float u, float v)
		:pos(x, y, z), texCoord(u, v)
	{

	}

	Vertex(float x, float y, float z, float r, float g, float b)
		:pos(x, y, z), color(r, g, b)
	{

	}



	DirectX::XMFLOAT3 GetPos()
	{
		return pos;
	}




	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 normal;

	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMFLOAT3 color;


	
};

struct Vertex2
{
	Vertex2() {}
	Vertex2(float tu2,float tv2)
		:texCoord2(tu2,tv2)
	{

	}

	DirectX::XMFLOAT2 texCoord2;



};

