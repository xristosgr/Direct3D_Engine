#include"ShadowRenderer.h"
#include <PxPhysicsAPI.h>

void ShadowRenderer::RenderShadowObjects(XMMATRIX& viewMatrix, XMMATRIX& ProjectionMatrix, VertexShader& vertexShader, PixelShader& pixelShader,bool frustum)
{

	this->deviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->deviceContext->PSSetSamplers(0, 1, this->samplerState_Clamp.GetAddressOf());
	this->deviceContext->PSSetSamplers(1, 1, this->samplerState_Wrap.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);




}
void ShadowRenderer::DrawShadowObject(GameObject& gameObject, Camera& camera, XMMATRIX& projectionMatrix, XMMATRIX& viewMatrix, FrustumClass& frustum, bool culling, float xSize, float ySize, float zSize, float distance)
{
	//Draw object with CPU frustum culling

	if (culling)
	{
		XMMATRIX viewProj = viewMatrix * projectionMatrix;

		frustum.ConstructFrustum(distance, viewMatrix, projectionMatrix);

		XMFLOAT3 tempPos = gameObject.GetPositionFloat3();
		

		bool check = frustum.CheckRect(tempPos.x, tempPos.y, tempPos.z, gameObject.scale.x, gameObject.scale.y, gameObject.scale.z);
		if (check)
		{
			XMFLOAT3 diff = XMFLOAT3(camera.GetPositionFloat3().x - gameObject.pos.x, camera.GetPositionFloat3().y - gameObject.pos.y, camera.GetPositionFloat3().z - gameObject.pos.z);

			
			physx::PxVec3 diffVec = physx::PxVec3(diff.x, diff.y, diff.z);

			float dist = diffVec.dot(diffVec);

			//OutputDebugStringA(("DIST = " + std::to_string(dist) + "\n").c_str());
			if (dist < shadowCullingRadius)
			{
				gameObject.Draw(viewMatrix, projectionMatrix);
			}
		}
	}
	else
	{
		gameObject.Draw(viewMatrix, projectionMatrix);
	}

}


void ShadowRenderer::RenderToTexture(std::vector<GameObject*>& gameObjects,Camera& camera, RenderTexture& shadowMap, PointLight& light, VertexShader& vertexshader, PixelShader& pixelShader, int index)
{
	shadowMap.SetRenderTarget(deviceContext.Get(), shadowMap.m_depthStencilView);
	shadowMap.ClearRenderTarget(deviceContext.Get(), shadowMap.m_depthStencilView, 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX viewMatrix = light.lightViewMatrix;
	XMMATRIX projectionMatrix = light.lightProjectionMatrix;


	RenderShadowObjects(viewMatrix, projectionMatrix, vertexshader, pixelShader);



	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!gameObjects[i]->model.deleted)
		{
			if (!gameObjects[i]->attachToCamera)
			{
				if (gameObjects[i]->model.isAnimated)
				{
					RenderShadowObjects(viewMatrix, projectionMatrix, depthAnimVS, DepthAnimPS);
				}
				else
				{
					RenderShadowObjects(viewMatrix, projectionMatrix, vertexshader, pixelShader);

				}
				DrawShadowObject(*gameObjects[i],camera, projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, 1, 1, 1, 500);
			}
		}
	}
}

void ShadowRenderer::RenderDynamicShadows(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera)
{
	for (int i = 0; i < pointLights.size(); i++)
	{
		this->cb_vs_lightViewsVertexShader.data.lightViewMatrix[i] = pointLights[i].lightViewMatrix;
		this->cb_vs_lightViewsVertexShader.data.lightProjectionMatrix[i] = pointLights[i].lightProjectionMatrix;
		pointLights[i].UpdateCamera();
	}

	cb_vs_lightViewsVertexShader.UpdateBuffer();

	if (shadowtest)
	{
		for (int j = 0; j < pointLights.size(); j++)
		{
			deviceContext->RSSetViewports(1, &pointLights[j].m_shadowMap.m_viewport);
			RenderToTexture(gameObjects, camera, pointLights[j].m_shadowMap, pointLights[j], depthVertexShader, depthPixelShader, j);
		}
	}

}

