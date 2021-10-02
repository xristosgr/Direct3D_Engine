#include "Render2D.h"

Render2D::Render2D()
{

}

bool Render2D::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,bool fullScreen,int screenSize,bool windowIsDebug)
{	//
	if (fullScreen)
	{
		if (screenSize == 0 || screenSize == 1024)
		{
			gameObject.SetScale(XMVECTOR{ 196.025f, 195.150f, 50.0f });
		}
		else if (screenSize == 2048)
		{
			gameObject.SetScale(XMVECTOR{ 3150.0f, 3100.0f, 50.0f });
		}
		else
		{
			gameObject.SetScale(XMVECTOR{ 306.5f, 174.0f, 50.0f });
		}
		//gameObject.SetRotation(XMVECTOR{ -1.570f, 1.665f,0.0f });
		//
		//gameObject.SetTranslation(XMVECTOR{ 0.0f, 2.255f, 20.0f });
	}
	else
	{
		gameObject.SetRotation(XMVECTOR{ -1.570f, 1.665f,0.0f });
		gameObject.SetTranslation(XMVECTOR{ 0.0f, 3.010f, 20.0f });
		gameObject.SetScale(XMVECTOR{ 99.085, 97.705, 50.0f });
	}
	if (windowIsDebug)
	{
		gameObject.SetRotation(XMVECTOR{ -1.570f, 1.665f,0.0f });
		gameObject.SetTranslation(XMVECTOR{ 100.0f, -150.010f, 20.0f });
		gameObject.SetScale(XMVECTOR{ 30., 30., 50.0f });
	}
	if (!gameObject.Initialize("Data\\Objects\\Rect.obj", device, deviceContext, cb_vs_vertexshader,false,false,false,false,false))
	{
		ErrorLogger::Log("Failed to load 2D texture.");
		return false;
	}
	gameObject.model.LoadModel("Data\\Objects\\Rect.obj", false);
	InitializeTexture(device, "data\\Textures\\image.jpeg", aiTextureType::aiTextureType_DIFFUSE);

	return true;
}

void Render2D::Draw(Camera& camera,ID3D11DeviceContext* deviceContext, ID3D11DepthStencilState* depthStencilState2D,XMMATRIX& viewMatrix2D, float fov,int windowWidth,int windowHeight,int screenNear,int screenFar)
{
	//deviceContext->PSSetShaderResources(0, 1, texture->GetTextureResourceViewAddress());
	camera.OrthographicFov(windowWidth, windowHeight, screenNear, screenFar);
	deviceContext->OMSetDepthStencilState(depthStencilState2D, 0);
	gameObject.DebugDraw(viewMatrix2D, camera.GetProjectionMatrix(),false);


	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight)*2, 0.1f, 100.0f);
}

GameObject* Render2D::GetGameObject()
{
	return &gameObject;
}

void Render2D::InitializeTexture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	texture = new Texture(device, filePath, type);
}
