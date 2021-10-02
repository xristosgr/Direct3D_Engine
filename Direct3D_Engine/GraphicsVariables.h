#pragma once
#include "FrustumClass.h"
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Camera.h"
#include "AppTimer.h"
#include "GameObject.h"
#include "Light.h"
#include "Render2D.h"
#include "RenderTexture.h"
#include "DeferredRenderTexture.h"
#include <future>
#include <thread>
#include"Shapes.h"
#include <vector>
#include"EnvironmentProbeMap.h"
#include"HeightfieldTerrain.h"


class GraphicsVariables
{
public:

	float rgb[4];


	XMMATRIX ViewMatrix2D, ViewMatrix2DHalf;




	int windowWidth = 0;
	int windowHeight = 0;
	//DX OBJECTS
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	D3D11_VIEWPORT viewport;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> forwardDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState2D;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState_Mask;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateWireframe;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateFront;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizerState;

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> AdditiveBlendState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> NoBlendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_Wrap;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_Clamp;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_ssao;

	//SHADERS

	VertexShader vertexshader;
	PixelShader pixelshader;
	PixelShader pixelshader_noLight;
	PixelShader postProccessPS;
	PixelShader SkyBloomPS;
	PixelShader BloomLightPS;
	PixelShader BloomBrightColorPS;
	PixelShader cubeMapPS;
	VertexShader depthVertexShader;
	PixelShader depthPixelShader;
	PixelShader lightPS;
	VertexShader SoftShadowsVS;
	VertexShader horizontalBlurVS;
	PixelShader horizontalBlurPS;
	VertexShader verticalBlurVS;
	PixelShader verticalBlurPS;
	PixelShader ShadowsShader;
	PixelShader IrradianceConvolutionPS;
	PixelShader PrifilterPS;
	PixelShader BrdfPS;
	PixelShader EnvironmentPS;
	PixelShader EnvironmentTransparentPS;

	VertexShader AnimatedVS;

	PixelShader DepthAnimPS;
	PixelShader ShadowAnimPS;
	VertexShader SkyVS;
	PixelShader SkyPS;
	PixelShader pbrPS;
	PixelShader transparentPbrPS;

	PixelShader EquirectangularToCubemapPS;
	VertexShader WaterVS;
	PixelShader WaterPS;
	PixelShader WaterBloomPS;

	VertexShader FPScameraVS;
	VertexShader FPSanimatedVS;
	PixelShader FPScameraPS;
	PixelShader ParticlePS;
	VertexShader m_shaderTextureVS;
	PixelShader m_shaderTexturePS;
	VertexShader depthAnimVS;
	VertexShader shadowVS;
	VertexShader shadowAnimVS;
	PixelShader materialPS;
	PixelShader material2PS;
	PixelShader material3PS;
	PixelShader GbufferPS;
	PixelShader deferredPS;
	VertexShader deferredVS;
	PixelShader SSAOPS;
	VertexShader SSAOVS;
	PixelShader ssrPS;
	VertexShader ssrVS;
	PixelShader debugPS;
	//Compute shaders
	ComputeShader computeShader;

	//CONSTANT BUFFERS
	ConstantBuffer<CB_Light_Global> cb_light_global;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_VS_lightViewsVertexShader> cb_vs_lightViewsVertexShader;
	ConstantBuffer<CB_VS_shadowParams> cb_vs_shadowParams;
	ConstantBuffer<CB_VS_blurWeights> cb_vs_blurWeights;
	ConstantBuffer<CB_VS_Reflection> cb_vs_reflection;
	ConstantBuffer<CB_VS_cameraBuffer> cb_vs_cameraParams;

	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_PS_sky> cb_ps_sky;
	ConstantBuffer<CB_PS_water> cb_ps_water;
	ConstantBuffer<CB_PS_PCF_Shadows> cb_ps_PCF_Shadows;
	ConstantBuffer<CB_CS_Forward_Plus> cb_cs_Forward_Plus;
	ConstantBuffer<CB_CS_Screen_To_View> CB_CS_screen_to_view;
	ConstantBuffer<CB_PS_Deferred_Shading> cb_ps_Deferred_Shading;
	ConstantBuffer<CB_PS_CAMERA> cb_ps_camera;
	ConstantBuffer<CB_PS_PBR> cb_ps_Pbr;
	ConstantBuffer< CB_PS_ScreenEffect> cb_ps_screenEffect;

	//WINDOWS AND RENDER TO TEXTURES
	Shapes cubeShape, cubeShapePBR, rect,rectSmall;
	
	Shapes fullScreenWidow;
	Render2D debugWindow;
	//std::vector<RenderTexture> shadowMap[6];
	std::vector<RenderTexture> cubeShadowMap;

	RenderTexture sceneTexture;
	RenderTexture cubeMap, cameraDepthTexture;
	RenderTexture environmentCubeMap;
	RenderTexture irradianceConvCubeMap;
	RenderTexture brdfCubeMap;
	RenderTexture prefilterCubeMap[5];
	
	RenderTexture bloomRenderTexture;
	RenderTexture BloomVerticalBlurTexture, BloomHorizontalBlurTexture,downSampleTexture;




	RenderTexture cubeText[6];


	std::vector<FrustumClass> frustums;


	XMFLOAT4 blurWeight1234 = XMFLOAT4(30.0f, 22.5f, 15.0f, 7.5f);
	XMFLOAT4 blurWeight5678 = XMFLOAT4(0.0f, 7.5f, 15.0f, 22.5f);
	float blurWeight9 = 30.0f;

	//ETC
	AppTimer timer;
	bool showDebugWindow = false;
	int debugWindowMode = 0;
	bool enableLights = true;

	float fov = 55.0f;

	//sky

	DirectX::XMFLOAT4 centerColor;
	DirectX::XMFLOAT4 apexColor;
	

	//Environment Probes
	RenderTexture CubeTextures[6];
	RenderTexture IrradianceConvCubeTextures[6];
	RenderTexture brdfTexture;
	RenderTexture PrifilterCubeTextures[6];
	std::vector <ID3D11Texture2D*> cubeTex;
	EnvironmentProbeMap probeMap, irradianceProbeMap;
	bool recalculateProbeMaps = false;


	int depthBias = 40;
	double slopeBias = 1;
	float clamp = 1.0f;


	bool shadowtest = true;
	float shadowCullingRadius = 1000.0f;
	bool switchView = false;
	bool enablePostProccess = true;

};