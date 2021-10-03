#include "PostProccessing.h"


void PostProccessing::BrdfRender(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader)
{
	texture.SetRenderTarget(deviceContext.Get(), texture.m_depthStencilView);

	texture.ClearRenderTarget(deviceContext.Get(), texture.m_depthStencilView, 1, 1, 1, 1);


	XMMATRIX viewMatrix = ViewMatrix2D;
	XMMATRIX projectionMatrix = camera.GetProjectionMatrix();


	RenderObjects(viewMatrix, projectionMatrix, vertexshader, pixelShader);


	this->deviceContext->RSSetState(this->rasterizerState.Get());


	this->deviceContext->PSSetShaderResources(0, 1, &environmentCubeMap.shaderResourceView);
	fullScreenWidow.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);
}

void PostProccessing::IrradianceConvolutionRender(Camera& camera, RenderTexture& currentView, int& index, VertexShader& vertexshader, PixelShader& pixelShader)
{

	currentView.SetRenderTarget(deviceContext.Get(), currentView.m_depthStencilView);

	currentView.ClearRenderTarget(deviceContext.Get(), currentView.m_depthStencilView, rgb[0], rgb[1], rgb[2], rgb[3]);

	XMMATRIX viewMatrix = irradianceProbeMap.camera[index].GetViewMatrix();
	XMMATRIX projectionMatrix = irradianceProbeMap.camera[index].GetProjectionMatrix();


	cb_ps_sky.data.apexColor = apexColor;
	cb_ps_sky.data.centerColor = centerColor;
	this->cb_ps_sky.UpdateBuffer();




	this->deviceContext->RSSetState(this->rasterizerState.Get());
	RenderObjects(viewMatrix, projectionMatrix, vertexshader, pixelShader);


	this->deviceContext->RSSetState(this->rasterizerState.Get());


	this->deviceContext->RSSetState(this->rasterizerStateFront.Get());
	cubeShape.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);


	this->deviceContext->PSSetShaderResources(0, 1, &environmentCubeMap.shaderResourceView);
	cubeShapePBR.DrawObject(camera, viewMatrix, projectionMatrix, false, true);

	this->deviceContext->RSSetState(this->rasterizerState.Get());
}

void PostProccessing::bloomEffect(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader)
{
	deviceContext->RSSetViewports(1, &this->viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), rgb);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
	texture.SetRenderTarget(deviceContext.Get(), texture.m_depthStencilView);
	texture.ClearRenderTarget(deviceContext.Get(), texture.m_depthStencilView, 0, 0, 0, 1.0f);

	XMMATRIX viewMatrix = camera.GetViewMatrix();
	XMMATRIX projectionMatrix = camera.GetProjectionMatrix();

	//ForwardRendering();
	RenderObjects(viewMatrix, projectionMatrix, this->vertexshader, BloomLightPS);
	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->BloomLightPS.GetShader(), NULL, 0);
	this->deviceContext->PSSetShaderResources(0, 1, &sceneTexture.shaderResourceView);

	fullScreenWidow.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);



	deviceContext->RSSetViewports(1, &this->viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), rgb);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	camera.PerspectiveFov(fov, static_cast<float>((windowWidth) / (windowHeight)) * 2, 0.1f, 10000.0f);

	viewMatrix = camera.GetViewMatrix();
	projectionMatrix = camera.GetProjectionMatrix();


	RenderObjects(viewMatrix, projectionMatrix, verticalBlurVS, verticalBlurPS);
	deviceContext->RSSetViewports(1, &BloomVerticalBlurTexture.m_viewport);

	BloomVerticalBlurTexture.SetRenderTarget(deviceContext.Get(), BloomVerticalBlurTexture.m_depthStencilView);
	BloomVerticalBlurTexture.ClearRenderTarget(deviceContext.Get(), BloomVerticalBlurTexture.m_depthStencilView, 0, 0, 0, 1.0f);

	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);

	this->deviceContext->PSSetShaderResources(0, 1, &bloomRenderTexture.shaderResourceView);
	rect.Draw(camera, depthStencilState2D.Get(), ViewMatrix2DHalf, camera.GetProjectionMatrix(), windowWidth, windowHeight);




	deviceContext->RSSetViewports(1, &this->viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), rgb);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);



	deviceContext->RSSetViewports(1, &BloomHorizontalBlurTexture.m_viewport);
	RenderObjects(viewMatrix, projectionMatrix, horizontalBlurVS, horizontalBlurPS);
	BloomHorizontalBlurTexture.SetRenderTarget(deviceContext.Get(), BloomHorizontalBlurTexture.m_depthStencilView);
	BloomHorizontalBlurTexture.ClearRenderTarget(deviceContext.Get(), BloomHorizontalBlurTexture.m_depthStencilView, 0, 0, 0, 1.0f);
	this->deviceContext->PSSetShaderResources(0, 1, &BloomVerticalBlurTexture.shaderResourceView);
	rect.Draw(camera, depthStencilState2D.Get(), ViewMatrix2DHalf, camera.GetProjectionMatrix(), windowWidth, windowHeight);


	deviceContext->RSSetViewports(1, &this->viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), rgb);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	cb_vs_vertexshader.data.viewMatrix = viewMatrix;
	cb_vs_vertexshader.data.projectionMatrix = projectionMatrix;
	cb_vs_vertexshader.UpdateBuffer();

	texture.SetRenderTarget(deviceContext.Get(), texture.m_depthStencilView);
	RenderObjects(viewMatrix, projectionMatrix, this->vertexshader, pixelshader_noLight);
	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);
	texture.ClearRenderTarget(deviceContext.Get(), texture.m_depthStencilView, 0, 0, 0, 1.0f);
	this->deviceContext->PSSetShaderResources(0, 1, &BloomHorizontalBlurTexture.shaderResourceView);
	fullScreenWidow.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);
}

void PostProccessing::RenderObjects(XMMATRIX& viewMatrix, XMMATRIX& ProjectionMatrix, VertexShader& vertexShader, PixelShader& pixelShader)
{

	this->deviceContext->IASetInputLayout(vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->deviceContext->PSSetSamplers(0, 1, this->samplerState_Clamp.GetAddressOf());
	this->deviceContext->PSSetSamplers(1, 1, this->samplerState_Wrap.GetAddressOf());
	this->deviceContext->PSSetSamplers(2, 1, this->samplerState_ssao.GetAddressOf());
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
}
