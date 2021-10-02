#pragma once
#include "NodeClass.h"
#include <vector>
#include<future>
#include <PxPhysicsAPI.h>
#include "GameObject.h"
#include <thread>

class GridClass
{
public:
	GridClass();
	void Initialize( Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void DrawGrid(Camera& camera, std::vector<NodeClass>& validNodes);
	void DrawBounds(Camera& camera);
	void FindNeighbours(NodeClass& currentNode, std::vector<NodeClass>& validNodes);
	void CreatePathGrid(std::vector<NodeClass>& validNodes);

	//Shapes gridBoundsShape;
	XMFLOAT3 bounds;
	XMFLOAT3 pos;

	std::vector<NodeClass> nodes;
	bool showGrid;
	bool isReady;
	bool bCreateMesh;


private:
	void InitThread();



	Microsoft::WRL::ComPtr<ID3D11Device> _device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _deviceContext;
	DirectX::XMMATRIX _transformMatrix;
	ConstantBuffer<CB_VS_vertexshader> _cb_vs_vertexshader;

	std::future<void> m_async;
};
