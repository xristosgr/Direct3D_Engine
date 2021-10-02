#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include"Mesh.h"
#include"Camera.h"

using namespace DirectX;

class Shapes
{
public:

	Shapes();
	void InitRect(const int sizeX, const int sizeY, Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool shadowsAlignment);
	void InitCube(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,bool visualize = true);

	void Draw(Camera& camera, ID3D11DepthStencilState* depthStencilState2D, const XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, int& windowWidth, int &windowHeight, float screenNear = 0.0f, float screenFar=1.0f, bool blenderModel = false, bool isTextured= false,bool isPBR = false);
	void DrawObject(Camera& camera,const XMMATRIX& viewMatrix, bool blenderModel, bool isTextured);
	void DrawRect(Camera& camera, const XMMATRIX& viewMatrix, bool blenderModel, bool isTextured);
	void DrawObject(Camera& camera, const XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, bool blenderModel, bool isTextured);
	void DrawGui(std::string name);


	void SetupCamera(int windowWidth, int windowHeight);

	Camera* GetCamera();

	void UpdateCamera(int& select);

	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotVector;
	DirectX::XMVECTOR posVector;

	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;
	XMFLOAT3 direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);


	XMMATRIX cameraViewMatrix;
	XMMATRIX cameraProjectionMatrix;


	Mesh* mesh;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;
	

	Camera camera;

	//int select = 0;

	XMFLOAT3 viewMatrices[6];



public:
	std::vector< Vertex> vertices;
	std::vector< UINT32> indices;


};

