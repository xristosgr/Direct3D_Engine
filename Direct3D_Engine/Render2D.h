#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Texture.h"

class Render2D
{
public:
	Render2D();

	bool Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext,ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool fullScreen, int screenSize=0, bool windowIsDebug = false);
	void Draw(Camera& camera, ID3D11DeviceContext* deviceContext, ID3D11DepthStencilState* depthStencilState2D, XMMATRIX& viewMatrix2D,float fov, int windowWidth, int windowHeight, int screenNear, int screenFar);
	GameObject* GetGameObject();
	Texture* texture;
private:

	void InitializeTexture(ID3D11Device* device, const std::string& filePath, aiTextureType type);
	
	GameObject gameObject;
};

