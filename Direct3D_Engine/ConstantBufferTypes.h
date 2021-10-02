#pragma once

#include <DirectXMath.h>
#include "ShaderSettings.hlsli"

struct CB_Light_Global
{
	unsigned int lightsSize;
};

struct CB_VS_vertexshader
{


	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;

	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;



	DirectX::XMMATRIX bones_transform[100];

};

struct CB_VS_lightViewsVertexShader
{

	DirectX::XMMATRIX lightViewMatrix[MAX_NO_OF_LIGHTS];
	DirectX::XMMATRIX lightProjectionMatrix[MAX_NO_OF_LIGHTS];
	
	//DirectX::XMMATRIX cascadedViewMatrix[4];
	//DirectX::XMMATRIX cascadedProjectionMatrix[4];
};

struct CB_VS_vertexshaderProjection
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;

	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projectionMatrix;

	DirectX::XMMATRIX ViewMatrix2;
	DirectX::XMMATRIX ProjectionMatrix2;



	DirectX::XMFLOAT3 cameraPos;

};
struct CB_VS_Reflection
{
	DirectX::XMMATRIX reflectionMatrix;
};
struct CB_VS_shadowParams
{
	float shadowSize;
	DirectX::XMFLOAT3 padding;
};
struct CB_VS_blurWeights
{
	DirectX::XMFLOAT4 weight1234;
	DirectX::XMFLOAT4 weight5678;
	float weight9;
	DirectX::XMFLOAT3 padding;
	bool test;
};
struct CB_VS_cameraBuffer
{
	DirectX::XMFLOAT4 cameraPos;

};


struct CB_PS_light
{
	DirectX::XMFLOAT4 ambientLightColorAndStrength;
	DirectX::XMFLOAT4 dynamicLightPosition[MAX_NO_OF_LIGHTS];
	
	DirectX::XMFLOAT4 dynamicLightAttenuationAndStrength[MAX_NO_OF_LIGHTS];
	DirectX::XMFLOAT4 dynamicLightColor[MAX_NO_OF_LIGHTS];

	DirectX::XMFLOAT4 lightDirectionAndSpecularPower[MAX_NO_OF_LIGHTS];
	DirectX::XMFLOAT4 specularColor[MAX_NO_OF_LIGHTS];

	DirectX::XMFLOAT4 sunLightPosition;
	DirectX::XMFLOAT4 sunLightColor;
	
	DirectX::XMFLOAT4 sunlightDirectionAndSpecularPower;
	DirectX::XMFLOAT4 sunspecularColor;



	int lightsNR;
	DirectX::XMFLOAT3 padding;

	DirectX::XMFLOAT4 enabledLightsAttribs[MAX_NO_OF_LIGHTS];


	bool enableLights;
	DirectX::XMFLOAT3 cameraPos;
	bool enableSunLight;

	


};

struct CB_PS_sky
{
	DirectX::XMFLOAT4 apexColor;
	DirectX::XMFLOAT4 centerColor;

};

struct CB_PS_water
{

	float waterTranslation;
};


struct CB_PS_PCF_Shadows
{

	int pcfLevel;
	double bias;
	bool enableShadows;
};

struct CB_CS_Screen_To_View
{
	DirectX::XMMATRIX InverseProjection;
	DirectX::XMFLOAT2 ScreenDimensions;

};
struct CB_CS_Forward_Plus
{
	DirectX::XMUINT3   numThreadGroups;
	DirectX::XMUINT3  numThreads;

};

struct CB_PS_Deferred_Shading
{
	DirectX::XMFLOAT4 Radius1[MAX_NO_OF_LIGHTS];
	DirectX::XMFLOAT4 cutOff[MAX_NO_OF_LIGHTS];

};

struct CB_PS_CAMERA
{
	DirectX::XMMATRIX projectionMatrix;
	DirectX::XMMATRIX viewMatrix;

};
struct CB_PS_PBR
{
	float roughness;

};
struct CB_PS_ScreenEffect
{
	float gamma;
	float bloomBrightness;
	float bloomStrenth;
};
