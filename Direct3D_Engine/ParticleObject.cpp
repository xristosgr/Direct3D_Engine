#include "ParticleObject.h"

ParticleObject::ParticleObject()
{
}

void ParticleObject::Initialize(const std::string filePath, const std::string textfilePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{

	if (!particle.Initialize(filePath, device, deviceContext, cb_vs_vertexshader, false, false, false, false, false))
	{
		return;
	}
	particle.model.LoadModel(filePath, false);

	texture.CreateTextureWIC(device, textfilePath);
}

void ParticleObject::Draw(const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured)
{
	particle.Draw(viewMatrix, projectionMatrix, true);
}
