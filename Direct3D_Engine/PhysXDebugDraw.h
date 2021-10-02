#pragma once
#include <d3d11.h>
#include <wrl.h>
#include "Vertex.h"
#include <vector>
#include "Mesh.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "Camera.h"
#include <PxPhysicsAPI.h>

class PhysXDebugDraw
{
public:

	PhysXDebugDraw();
	void DebugDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader, const physx::PxDebugLine& line,Camera& camera);
	
public:
	Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
	std::vector<Vertex> vertices;
	Mesh* mesh;
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	ConstantBuffer<CB_VS_vertexshader>* m_cb_vs_vertexshader;
};

