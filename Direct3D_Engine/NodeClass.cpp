#include "NodeClass.h"

NodeClass::NodeClass()
{
	//isActive = false;
	//isValidPath = true;
	//isVisited = false;
	//parent = nullptr;
}

void NodeClass::Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool visualize)
{
	isActive = false;
	isValidPath = true;
	isVisited = false;
	parent = nullptr;

	//shape.InitCube(device, deviceContext, transformMatrix, cb_vs_vertexshader,true);
				
	scale = DirectX::XMFLOAT3(0.3f, 0.3f, 0.3f);
	//shape.scale = scale;
}

void NodeClass::DrawNode(Camera& camera)
{
	//shape.DrawObject(camera, camera.GetViewMatrix(), false, false);

	if (isVisited)
	{
		//shape.DrawObject(camera, camera.GetViewMatrix(), false, false);

		//for (int i = 0; i < neighbours.size(); ++i)
		//{
		//	neighbours[i]->shape.DrawObject(camera, camera.GetViewMatrix(), false, false);
		//}
		//OutputDebugStringA(("SIZE = " + std::to_string(neighbours.size()) + "\n").c_str());
	}

}
