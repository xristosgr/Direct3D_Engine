#pragma once
#include <d3d11.h>
#include "COMException.h"
#include "ErrorLogger.h"
#include<vector>
#include<DirectXMath.h>
#include"GameObject.h"

class EnvironmentProbeMap : virtual public GameObject
{
public:
	EnvironmentProbeMap();
	bool InitializeProbe(ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void SetupCamera(int& windowWidth, int& windowHeight);
	Camera* GetCamera();
	void UpdateCamera(int& windowWidth, int& windowHeight, float fov = 90.0f);
	void DrawGui(std::string name);

	DirectX::XMMATRIX viewMatrices[6];
	DirectX::XMMATRIX projectionMatrices[6];
	Camera camera[6];
	DirectX::XMFLOAT3 direction[6];
	DirectX::XMFLOAT3 cameraPos;
	bool recalculate = false;
private:
	std::string name = "Light attributes";

	bool windowActive = false;
};

