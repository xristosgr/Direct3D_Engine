#pragma once
#include "GraphicsVariables.h"

class ShadowRenderer: public GraphicsVariables
{
protected:
	void RenderToTexture(std::vector<GameObject*>& gameObjects, Camera& camera, RenderTexture& shadowMap, PointLight& light, VertexShader& vertexshader, PixelShader& pixelShader,int index);
	void RenderDynamicShadows(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera);

	void RenderShadowObjects(XMMATRIX&, XMMATRIX&, VertexShader& vertexshader, PixelShader& pixelShader, bool frustum = false);
	void DrawShadowObject(GameObject& gameObject, Camera& camera, XMMATRIX& projectionMatrix, XMMATRIX& viewMatrix, FrustumClass& frustum, bool culling, float xSize, float ySize, float zSize, float distance);
};