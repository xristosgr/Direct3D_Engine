#pragma once
//#include "GameObject.h"
#include "Camera.h"
#include "AppTimer.h"
#include "ModelLoader.h"
#include "RenderTexture.h"

class Light 
{
public:
	Light();
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void DrawGui(std::string name);
	void Draw(XMMATRIX& viewMatrix, XMMATRIX projectionMatrix);

	std::string lightname = "nonShadowLight";
	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrenth = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
	float lightAttenuation = 1.0f;

	DirectX::XMFLOAT3 specularColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float specularPower = 1.0f;


	bool isLightEnabled = true;
	bool bRenderModel = false;
	float radius;
	ModelLoader model;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;


private:
	std::string name = "Light attributes";
	
	bool windowActive = false;
	

};

class SunLight 
{
public:
	SunLight();
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void DrawGui(std::string name);
	void SetupCamera();
	//Camera* GetCamera();
	void UpdateCamera();

	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrenth = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;

	DirectX::XMFLOAT3 specularColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float specularPower = 1.f;
	DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

	int count;
	XMMATRIX lightViewMatrix[4];
	XMMATRIX lightProjectionMatrix[4];
	XMFLOAT3 posOffset[4];
	XMFLOAT3 dirOffset;
	Camera camera[4];
private:
	std::string name = "Light attributes";

	bool windowActive = false;

public:

	float orthoNear[4];
	float orthoFar[4];
	int orthoX[4];
	int	orthoY[4];

	
	XMFLOAT3 sunPosOffset = XMFLOAT3(0,0,0);
};



class PointLight
{
public:
	PointLight();
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	virtual void DrawGui(std::string name) ;
	void Draw(XMMATRIX& viewMatrix, XMMATRIX projectionMatrix);
	void SetupCamera(int windowWidth, int windowHeight);
	//Camera* GetCamera();
	void UpdateCamera();

	std::string lightname = "Light";
	DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float lightStrenth = 1.0f;
	float attenuation_a = 1.0f;
	float attenuation_b = 0.1f;
	float attenuation_c = 0.1f;
	float lightAttenuation = 1.0f;

	DirectX::XMFLOAT3 specularColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float specularPower = 1.0f;
	DirectX::XMFLOAT3 direction;
	DirectX::XMFLOAT3 offset;

	XMMATRIX lightViewMatrix;
	XMMATRIX lightProjectionMatrix;
	XMFLOAT3 posOffset;
	Camera camera;

	float radius;
	float cutOff;
	float lightType;

	bool isAttached;
public:
	bool isLightEnabled = true;
	std::string name = "Light attributes";

	bool windowActive = false;

	float fov = 125.50f;
	float dimensions = 2;
	float nearZ = 0.1f;
	float farZ = 1000.0f;

	AppTimer timer;
	bool bRenderModel = false;
	bool bShadow = true;
	ModelLoader model;

	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;


	RenderTexture m_shadowMap;
};