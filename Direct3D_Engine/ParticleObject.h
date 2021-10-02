#pragma once
#include"GameObject.h"
#include "Texture.h"

class ParticleObject
{
public:
	ParticleObject();
	void Initialize(const std::string filePath, const std::string textfilePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void Draw(const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured);
	GameObject particle;
	Texture texture;

	float delay = 0.0f;
};

