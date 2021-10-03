#pragma once

#include "GraphicsVariables.h"

class PostProccessing : virtual public GraphicsVariables
{
public:

	//void RenderSceneToTexture(float& dt,ID3D11DeviceContext* deviceContext,RenderTexture& sceneTexture, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera, float* rgb);
	void BrdfRender(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader);

	void IrradianceConvolutionRender(Camera& camera, RenderTexture& currentView, int& index, VertexShader& vertexshader, PixelShader& pixelShader);
	void bloomEffect(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader);
	void AmbientOcclusion(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader);

private:
	void RenderObjects(XMMATRIX&, XMMATRIX&, VertexShader& vertexshader, PixelShader& pixelShader);
};

