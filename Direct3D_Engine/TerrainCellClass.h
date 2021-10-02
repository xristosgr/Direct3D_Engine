#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include<vector>

class TerrainCellClass
{
private:
	struct ModelType
	{
		float x;
		float y;
		float z;

		float tu;
		float tv;

		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;

		float tu2;
		float tv2;
	};
	struct VectorType
	{
		float x, y, z;
	};

	//struct ColorVertexType
	//{
	//	DirectX::XMFLOAT3 position;
	//	DirectX::XMFLOAT4 color;
	//};

public:
	TerrainCellClass();
	bool Initialize(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth, int terrainWidth);
	void Render(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetIndexCount();
	int GetLineBuffersIndexCount();
	void GetCellDimensions(float&, float&, float&, float&, float&, float&);


	bool InitializeBuffers(ID3D11Device*, int, int, int, int, int, ModelType*);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);
	void CalculateCellDimensions();
	bool BuildLineBuffers(ID3D11Device*);
	void RenderLineBuffers(ID3D11DeviceContext* deviceContext);

	void DeleteCells();

public:
	VectorType* m_vertexList;
private:
	int m_vertexCount, m_indexCount, m_lineIndexCount;
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer, * m_lineVertexBuffer, * m_lineIndexBuffer;
	ID3D11Buffer* m_vertexBuffer2,* m_lineColorvertexBuffer;

	float m_maxWidth, m_maxHeight, m_maxDepth, m_minWidth, m_minHeight, m_minDepth;
	

public:
	float m_positionX, m_positionY, m_positionZ;
	std::vector< Vertex> vertices;
	
	std::vector< Vertex2> vertex2; 
	std::vector< unsigned long> indices;
	std::vector< unsigned long> debugIndices;


	bool deleted = false;

};

