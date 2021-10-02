#pragma once
#include <d3d11.h>
#include "COMException.h"
#include "ErrorLogger.h"
#include <vector>
#include <DirectXMath.h>

const UINT bufferSize = 3;
class DeferredRenderTexture
{
private:
	
public:

	bool Initialize(ID3D11Device* device, int textureWidth, int textureHeight);
	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView(int index);
	ID3D11ShaderResourceView* shaderResourceView[bufferSize];




private:

	//ID3D11ShaderResourceView* m_shaderResourceView;
public:
	ID3D11Texture2D* pTexture;
	ID3D11Texture2D* m_renderTargetTexture[bufferSize];
	ID3D11RenderTargetView* m_renderTargetView[bufferSize];
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_texture;
	D3D11_VIEWPORT m_viewport;

	bool init = false;
};