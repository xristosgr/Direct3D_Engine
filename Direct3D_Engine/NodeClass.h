#pragma once
//#include "Mesh.h"
//#include "Shapes.h"
//#include <map>
#include "GameObject.h"

class NodeClass
{
public:
	NodeClass();
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,bool visualize = true);
	void DrawNode(Camera& camera);

	bool isActive;
	bool isValidPath;
	bool isVisited;

	float gCost;
	float hCost;
	float fCost;

	std::vector<NodeClass*> neighbours;
	NodeClass* parent;

	XMFLOAT3 pos;
	XMFLOAT3 scale;
	//Shapes shape;

};
