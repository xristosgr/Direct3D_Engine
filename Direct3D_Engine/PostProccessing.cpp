#include "PostProccessing.h"

//void PostProccessing::RenderSceneToTexture()
//{
//
//	sceneTexture.SetRenderTarget(deviceContext.Get(), sceneTexture.m_depthStencilView);
//	sceneTexture.ClearRenderTarget(deviceContext.Get(), sceneTexture.m_depthStencilView, rgb[0], rgb[1], rgb[2], rgb[3]);
//
//
//
//	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
//	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
//
//
//	XMMATRIX viewMatrix = camera.GetViewMatrix();
//	XMMATRIX projectionMatrix = camera.GetProjectionMatrix();
//
//
//	deviceContext->PSSetShaderResources(4, 1, &irradianceConvCubeMap.shaderResourceView);
//	deviceContext->PSSetShaderResources(5, 1, &brdfTexture.shaderResourceView);
//	deviceContext->PSSetShaderResources(6, 1, &prefilterCubeMap[0].shaderResourceView);
//	deviceContext->PSSetShaderResources(7, 1, &prefilterCubeMap[1].shaderResourceView);
//	deviceContext->PSSetShaderResources(8, 1, &prefilterCubeMap[2].shaderResourceView);
//	deviceContext->PSSetShaderResources(9, 1, &prefilterCubeMap[3].shaderResourceView);
//	deviceContext->PSSetShaderResources(10, 1, &prefilterCubeMap[4].shaderResourceView);
//	deviceContext->PSSetShaderResources(11, 1, &BloomVerticalBlurTexture.shaderResourceView);
//	std::vector< ID3D11ShaderResourceView*> ShadowTextures;
//	ShadowTextures.resize(shadowMap.size());
//	int index = 0;
//	for (int j = 0; j < shadowMap.size(); ++j)
//	{
//
//		ShadowTextures[index] = shadowMap[j].shaderResourceView;
//		index++;
//
//
//
//	}
//
//	deviceContext->PSSetShaderResources(12, pointLights.size(), ShadowTextures.data());
//
//	RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, pbrPS);
//
//
//	for (int i = 0; i < gameObjects.size(); ++i)
//	{
//		if (!gameObjects[i]->model.deleted)
//		{
//
//			if (!gameObjects[i]->isTransparent)
//			{
//
//				this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
//				if (gameObjects[i]->model.isAnimated)
//				{
//					RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, pbrPS);
//
//
//					if (gameObjects[i]->attachToCamera)
//					{
//						RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, pbrPS);
//					}
//				}
//				else
//				{
//					if (gameObjects[i]->attachToCamera)
//					{
//						RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, pbrPS);
//					}
//					else
//					{
//						RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, pbrPS);
//					}
//				}
//
//
//
//				if (!gameObjects[i]->isTextured)
//				{
//					this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
//					this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
//					this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
//					this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
//				}
//
//				if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
//				{
//					if (camera.PossessCharacter)
//					{
//						DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
//					}
//
//				}
//				else
//				{
//					DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
//				}
//			}
//
//		}
//	}
//
//
//	RenderObjects(viewMatrix, projectionMatrix, SkyVS, SkyPS);
//	cb_ps_sky.data.apexColor = apexColor;
//	cb_ps_sky.data.centerColor = centerColor;
//	this->cb_ps_sky.UpdateBuffer();
//	this->deviceContext->RSSetState(this->rasterizerStateFront.Get());
//	this->deviceContext->VSSetShader(SkyVS.GetShader(), NULL, 0);
//	this->deviceContext->PSSetShader(SkyPS.GetShader(), NULL, 0);
//	skydomeObject.scale = XMFLOAT3(500, 500, 500);
//	skydomeObject.SetTranslation(camera.GetPositionFloat3());
//	skydomeObject.Draw(viewMatrix, projectionMatrix, false);
//
//
//	this->deviceContext->RSSetState(this->rasterizerState.Get());
//
//	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
//
//	RenderObjects(viewMatrix, projectionMatrix, vertexshader, lightPS);
//	for (int i = 0; i < pointLights.size(); ++i)
//	{
//		//if (lightNames[listbox_light_current] == pointLights[i].lightname)
//		//{
//			//if(pointLights[i].bRenderModel && pointLights[i].model.vertices.size()>0)
//		pointLights[i].Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix());
//		//}
//	}
//
//	for (int i = 0; i < gameObjects.size(); ++i)
//	{
//		if (!gameObjects[i]->model.deleted)
//		{
//
//			if (gameObjects[i]->isTransparent)
//			{
//
//				this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
//				if (gameObjects[i]->model.isAnimated)
//				{
//					RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, transparentPbrPS);
//					if (gameObjects[i]->attachToCamera)
//					{
//						RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, transparentPbrPS);
//					}
//				}
//				else
//				{
//					if (gameObjects[i]->attachToCamera)
//					{
//						RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, transparentPbrPS);
//					}
//					else
//					{
//						RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, transparentPbrPS);
//					}
//
//				}
//
//				if (!gameObjects[i]->isTextured)
//				{
//					this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
//					this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
//					this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
//					this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
//				}
//
//				if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
//				{
//					if (camera.PossessCharacter)
//					{
//						DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
//					}
//
//				}
//				else
//				{
//					DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
//				}
//			}
//
//		}
//
//
//	}
//
//	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
//
//
//
//
//
//	this->deviceContext->OMSetBlendState(AdditiveBlendState.Get(), NULL, 0xFFFFFFFF);
//
//	//if (camera.PossessCharacter)
//	//{
//
//	if (muzzleFlash.particle.attachToCamera)
//	{
//		RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, ParticlePS);
//		this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
//		this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
//	}
//	else
//	{
//		RenderObjects(viewMatrix, projectionMatrix, vertexshader, ParticlePS);
//		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
//		this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
//	}
//	muzzleFlash.particle.attachToCamera = true;
//	muzzleFlash.particle.scale = XMFLOAT3(0.1, 0.2, 0.2);
//	muzzleFlash.particle.rot = XMFLOAT3(0.0, 1.550, 0.0);
//	muzzleFlash.particle.pos = XMFLOAT3(0.145, -0.110f, 0.430);
//	muzzleFlash.particle.frustumEnable = false;
//	for (int j = 0; j < gameObjects.size(); ++j)
//	{
//		if (gameObjects[j]->isFiring)
//		{
//			deviceContext->PSSetShaderResources(0, 1, muzzleFlash.texture.GetTextureResourceViewAddress());
//			muzzleFlash.Draw(viewMatrix, projectionMatrix, true);
//
//		}
//	}
//
//	//		this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
//	//		if (crosshair.particle.attachToCamera)
//	//		{
//	//			RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, ParticlePS);
//	//			this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
//	//			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
//	//		}
//	//		else
//	//		{
//	//			RenderObjects(viewMatrix, projectionMatrix, vertexshader, ParticlePS);
//	//			this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
//	//			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
//	//		}
//	//
//	//
//	//
//	//
//	//		deviceContext->PSSetShaderResources(0, 1, crosshair.texture.GetTextureResourceViewAddress());
//	//
//	//		crosshair.particle.attachToCamera = true;
//	//		crosshair.particle.scale = XMFLOAT3(0.005, 0.005, 0.005);
//	//		crosshair.particle.rot = XMFLOAT3(0.0, 1.550, 0.0);
//	//		crosshair.particle.pos = XMFLOAT3(0.0, 0.0f, 0.110);
//	//		crosshair.particle.frustumEnable = false;
//	//		if (camera.PossessCharacter)
//	//			crosshair.Draw(viewMatrix, projectionMatrix, true);
//
//		//}
//
//	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
//}
//
//void PostProccessing::bloomEffect()
//{
//
//	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
//	bloomRenderTexture.SetRenderTarget(deviceContext.Get(), bloomRenderTexture.m_depthStencilView);
//	bloomRenderTexture.ClearRenderTarget(deviceContext.Get(), bloomRenderTexture.m_depthStencilView, 0, 0, 0, 1.0f);
//
//	XMMATRIX viewMatrix = camera.GetViewMatrix();
//	XMMATRIX projectionMatrix = camera.GetProjectionMatrix();
//
//	//ForwardRendering();
//	RenderObjects(viewMatrix, projectionMatrix, this->vertexshader, BloomLightPS);
//	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
//	this->deviceContext->PSSetShader(this->BloomLightPS.GetShader(), NULL, 0);
//	this->deviceContext->PSSetShaderResources(0, 1, &sceneTexture.shaderResourceView);
//	rect.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);
//
//
//
//
//	RenderObjects(viewMatrix, projectionMatrix, verticalBlurVS, verticalBlurPS);
//	BloomVerticalBlurTexture.SetRenderTarget(deviceContext.Get(), BloomVerticalBlurTexture.m_depthStencilView);
//	BloomVerticalBlurTexture.ClearRenderTarget(deviceContext.Get(), BloomVerticalBlurTexture.m_depthStencilView, 0, 0, 0, 1.0f);
//
//	this->deviceContext->PSSetShaderResources(0, 1, &bloomRenderTexture.shaderResourceView);
//	rect.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);
//
//	RenderObjects(viewMatrix, projectionMatrix, horizontalBlurVS, horizontalBlurPS);
//	bloomRenderTexture.SetRenderTarget(deviceContext.Get(), bloomRenderTexture.m_depthStencilView);
//	bloomRenderTexture.ClearRenderTarget(deviceContext.Get(), bloomRenderTexture.m_depthStencilView, 0, 0, 0, 1.0f);
//
//	this->deviceContext->PSSetShaderResources(0, 1, &BloomVerticalBlurTexture.shaderResourceView);
//	rect.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);
//}
