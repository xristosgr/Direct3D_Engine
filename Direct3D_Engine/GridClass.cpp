#include "GridClass.h"

GridClass::GridClass()
{
	showGrid = false;
	isReady = false;
	bCreateMesh = false;

	pos = XMFLOAT3(0, 0, 0);
}

void GridClass::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	_device = device;
	_deviceContext = deviceContext;
	_transformMatrix = transformMatrix;
	_cb_vs_vertexshader = cb_vs_vertexshader;

	m_async = std::async(std::launch::async, &GridClass::InitThread, this);
	//InitThread();

}


void GridClass::DrawGrid(Camera& camera, std::vector<NodeClass>& validNodes)
{
	//gridBoundsShape.DrawObject(camera, camera.GetViewMatrix(), false, false);
	//
	if (showGrid)
	{
		for (int i = 0; i < validNodes.size(); ++i)
		{
			validNodes[i].DrawNode(camera);
		}
	
		
	}
	
}

void GridClass::FindNeighbours(NodeClass& currentNode, std::vector<NodeClass>& validNodes)
{
	for (int i = 0; i < validNodes.size(); ++i)
	{

		if (currentNode.pos.x >= validNodes[i].pos.x - 1 && currentNode.pos.x <= validNodes[i].pos.x + 1 &&
			currentNode.pos.y >= validNodes[i].pos.y - 1 && currentNode.pos.y <= validNodes[i].pos.y + 1 &&
			currentNode.pos.z >= validNodes[i].pos.z -1 && currentNode.pos.z <= validNodes[i].pos.z +1)
		{
			currentNode.neighbours.push_back(&validNodes[i]);
		}
		
	
		//OutputDebugStringA(("Size = " + std::to_string(nodes[i].neighbours.size()) + "\n").c_str());
	}
	
	//OutputDebugStringA(("Size = " + std::to_string(currentNode.neighbours.size()) + "\n").c_str());

}

void GridClass::CreatePathGrid(std::vector<NodeClass>& validNodes)
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].isValidPath)
		{
			validNodes.push_back(nodes[i]);
		}
	}
	//OutputDebugStringA(("SIZE = " + std::to_string(validNodes.size()) + "\n").c_str());

	for (int i = 0; i < validNodes.size(); ++i)
	{

		if (validNodes[i].isValidPath)
			FindNeighbours(validNodes[i], validNodes);
	}
}

void GridClass::InitThread()
{
	//gridBoundsShape.InitCube(_device, _deviceContext, _transformMatrix, _cb_vs_vertexshader,false);
	bounds = DirectX::XMFLOAT3(140.0f, 12.0f, 140.0f);

	for (int y = -bounds.y; y < bounds.y; y++)
	{
		for (int z = -bounds.z; z < bounds.z; ++z)
		{
			for (int x = -bounds.x; x < bounds.x; ++x)
			{
				nodes.push_back(NodeClass());
				nodes[nodes.size() - 1].Initialize(_device, _deviceContext, _transformMatrix, _cb_vs_vertexshader,true);
				nodes[nodes.size() - 1].pos = DirectX::XMFLOAT3(pos.x - x, y, pos.z - z);

				//nodes[nodes.size() - 1].shape.pos = nodes[nodes.size() - 1].pos;
			}
		}
	}
	isReady = true;


	//OutputDebugStringA(("SIZE OF NODE = " + std::to_string(sizeof(NodeClass)) + "\n").c_str());
}
