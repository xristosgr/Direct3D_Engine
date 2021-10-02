#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include"Mesh.h"
#include"Camera.h"
#include "AppTimer.h"
#include<string>
#include <fstream>
#include<random>
#include<stdio.h>
#include "Vertex.h"
#include "TerrainCellClass.h"
#include "FrustumClass.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>

class HeightfieldTerrain
{
private:
	struct bulletObject
	{
		int id;
		bool hit;
		btRigidBody* body;
		bulletObject(btRigidBody* b, int i) : body(b), id(i), hit(false) {};
	};

	struct HeightMapType
	{
		float x;
		float y;
		float z;

		float nx, ny, nz;
		float r, g, b;
	};
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
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	float WavesHeightCalculator(int& i);
	float WavesHorizontalCalculator(int& i);
	float WavesVerticalCalculator(int& i);
	float dotProduct(float& x1, float& y1, float& x2, float& y2);
	double Drand(double min, double max);

public:
	bool RenderCell(int, FrustumClass& frustum);
	void RenderCellLines(int);
	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

private:

	bool LoadSetupFile(const char*);
	bool LoadRawHeightMap();
	bool LoadBitMapHeightMap();
	void SetTerrainCoordinates();
	bool LoadColorMap();
	bool CalculateNormals();
	bool BuildTerrainModel();
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	bool LoadTerrainCells();
	
	void DeleteHeightfield();

	btRigidBody* createCollision(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);

public:


	bool LoadData();
	bool InitHeightfield(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void UpdateHeightfield(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	void DrawHeightfield(Camera& camera, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool blenderModel, bool isTextured);
	void DrawGui(std::string name);



	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotVector;
	DirectX::XMVECTOR posVector;

	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;



private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;

	AppTimer timer;
	float yOffset = 0.0f;

	float* randomOffsetsY;
	float* randomOffsetsX;
	float* randomOffsetsZ;

	int indexNormal = 0;
public:

	//std::vector< ColorVertex> color;


	int m_terrainWidth = 24;
	int m_terrainHeight = 24;
	//float* HorizontalData;
	//float* HeightData;
	long double waveSpeed = 0;
	long double waveSpeedH = 0;
	long double waveSpeedV = 0;
	float waveOffset = 0.002f;
	float waveOffsetH = 0.002f;
	float waveOffsetV = 0.002f;
	float waveHeight = 1.0;
	float waveDistance = 67.0;
	float waveDistanceH = 67.0;
	float waveDistanceV = 67.0;


	bool enableWireFrame = false;
private:
	float steepness;
	float* amplitude;
	XMFLOAT3* direction;
	XMFLOAT3* w;
	float* phase_const;
	
	std::string filename = "Data/Heightmaps/Heightmap.bmp";

	std::ifstream inf;

	std::vector<float> tempVals;
	bool randInit = false;
	float randNum = 0;
	int waveCount = 12;

	float nX = 0;
	float nY = 1;
	float nZ = 0;

	float tX = nX;
	float tY = nY;
	float tZ = nZ;

	float bX = tX;
	float bY = tY;
	float bZ = tZ;


	int duSubdivide = 10;
	int dvSubdivide = 10;




	int m_indexCount = 0;
	//int index = 0;
	int k = 0;

	XMFLOAT3 Position;



	private:
		int m_heightScale;
		char* m_terrainFilename, * m_colorMapFilename;
		HeightMapType* m_heightMap;
		ModelType* m_terrainModel;

		ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
		ID3D11Buffer* m_ColorvertexBuffer;
		
		unsigned char* bitmapImage;
	public:
		FILE* filePtr;

		int m_vertexCount = 0;
		float* heightData;

		XMFLOAT3* vertex1;
		XMFLOAT3* vertex2;
		XMFLOAT3* vertex3;
		XMFLOAT3* vertex4;
		XMFLOAT3* vertex5;
		XMFLOAT3* vertex6;

		UINT* index1;
		UINT* index2;
		UINT* index3;
		UINT* index4;

	private:
		TerrainCellClass* m_TerrainCells;
		int m_cellCount;

		std::vector<FrustumClass> frustums;
		bool enableFrustum = true;


	private:
			btDefaultMotionState* motion;

			bool deleteCells = false;
};

