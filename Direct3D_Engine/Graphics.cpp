#include "Graphics.h"
#include <math.h>

bool Graphics::Initialize(HWND hwnd, Camera& camera, int width, int height)
{
	rgb[0] = 0.1f;
	rgb[1] = 0.1f;
	rgb[2] = 0.1f;
	rgb[3] = 1.0f;

	//inName = nullptr;
	//this->apexColor = XMFLOAT4{ 0.690f, 0.690f, 1.0f,1.0f };
	//this->centerColor = XMFLOAT4{ 0.985f, 0.785f, 0.775f,1.0f };

	vSync = 1;
	TextureInit = false;

	timer.Start();

	this->windowWidth = width;
	this->windowHeight = height;
	if (!InitializeDirectX(hwnd))
		return false;

	if (!InitializeShaders())
		return false;

	if (!InitializeScene(camera))
		return false;


	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{

		HRESULT hr;

		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		UINT MSSA_COUNT = 1;
		UINT MSSA_Quality = 0;
		

		//hr = device->CheckMultisampleQualityLevels(scd.BufferDesc.Format, scd.SampleDesc.Count, &maxQuality);

		scd.SampleDesc.Count = MSSA_COUNT;
		scd.SampleDesc.Quality = MSSA_Quality;


		//scd.SampleDesc.Quality = 0;
		//scd.SampleDesc.Count = 1;
		scd.BufferDesc.Width = this->windowWidth;
		scd.BufferDesc.Height = this->windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		//scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		scd.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		//scd.BufferDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	
		
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;//Set to false for full screen
		//scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;
		

		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			NULL, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			this->swapchain.GetAddressOf(), //Swapchain Address
			this->device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->deviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");


		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");


		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight, 1U, 1U, D3D11_BIND_DEPTH_STENCIL);
		depthStencilDesc.SampleDesc.Count = MSSA_COUNT;
		depthStencilDesc.SampleDesc.Quality = MSSA_Quality;

		//depthStencilDesc.SampleDesc.Count = 1;
		//depthStencilDesc.SampleDesc.Quality = 0;
		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (MSSA_COUNT > 1)
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		}
		
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), &depthStencilViewDesc, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());


		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		CD3D11_DEPTH_STENCIL_DESC depthstencildesc_mask(D3D11_DEFAULT);
		depthstencildesc_mask.DepthEnable = FALSE;
		depthstencildesc_mask.StencilEnable = TRUE;
		depthstencildesc_mask.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

		depthstencildesc_mask.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;

		hr = this->device->CreateDepthStencilState(&depthstencildesc_mask, this->depthStencilState_Mask.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//DepthStencil state for 2D rendering
		depthstencildesc.DepthEnable = false;
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState2D.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state 2D.");

		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//depthstencildesc.StencilEnable = FALSE;
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->forwardDepthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		//Create the Viewport and Set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));

		this->viewport = viewport;
		this->deviceContext->RSSetViewports(1, &this->viewport);

		//Create Rasterizer State

		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

		if (MSSA_COUNT > 1)
		{
			rasterizerDesc.MultisampleEnable = true;
			rasterizerDesc.AntialiasedLineEnable = true;
		}
		else
		{
			rasterizerDesc.MultisampleEnable = false;
			rasterizerDesc.AntialiasedLineEnable = false;
		}
		//rasterizerDesc.DepthBias = depthBias;
		//rasterizerDesc.DepthBiasClamp = clamp;
		//rasterizerDesc.SlopeScaledDepthBias = slopeBias;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerStateWireframe.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerStateFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create blend state
		D3D11_BLEND_DESC blendDesc = { 0 };
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };

		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0] = rtbd;
		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());

		//rtbd.BlendEnable = true;
		//rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		//rtbd.DestBlend = D3D11_BLEND_SRC_ALPHA;
		//rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		//rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		//rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
		//rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_ONE;
		rtbd.DestBlend = D3D11_BLEND_ONE;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0] = rtbd;

		hr = this->device->CreateBlendState(&blendDesc, this->AdditiveBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");
		rtbd.BlendEnable = false;
		blendDesc.RenderTarget[0] = rtbd;
		hr = this->device->CreateBlendState(&blendDesc, this->NoBlendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		
		sampDesc.BorderColor[0] = 0;
		sampDesc.BorderColor[1] = 0;
		sampDesc.BorderColor[2] = 0;
		sampDesc.BorderColor[3] = 0;
		//sampDesc.MaxAnisotropy = 1;
		sampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		//sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		//sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.MipLODBias = 0.0f;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_Wrap.GetAddressOf()); //Create sampler state wrap
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;



		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_Clamp.GetAddressOf()); //Create sampler state clamp
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;


		//sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		//sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		//sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState_ssao.GetAddressOf()); //Create sampler state clamp
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");


		this->deviceContext->VSSetSamplers(0, 1, this->samplerState_Wrap.GetAddressOf());
		this->deviceContext->VSSetSamplers(1, 1, this->samplerState_Wrap.GetAddressOf());





		std::vector<std::string> texpaths;
		texpaths.push_back("Data/CubeMaps/negx.jpg");//NegX
		texpaths.push_back("Data/CubeMaps/posx.jpg");//POSX
		texpaths.push_back("Data/CubeMaps/posy.jpg");//UP
		texpaths.push_back("Data/CubeMaps/negy.jpg");//DOWN
		texpaths.push_back("Data/CubeMaps/negz.jpg");//POSZ
		texpaths.push_back("Data/CubeMaps/posz.jpg");//NEGZ



		for (int i = 0; i < 6; ++i)
		{
			CubeTextures[i].Initialize(device.Get(), 512, 512);
			IrradianceConvCubeTextures[i].Initialize(device.Get(), 32, 32);

		}

		brdfTexture.Initialize(device.Get(), 512, 512);


		sceneTexture.Initialize(device.Get(), windowWidth, windowHeight);

		BloomHorizontalBlurTexture.Initialize(device.Get(), 640, 360);
		BloomVerticalBlurTexture.Initialize(device.Get(), 640, 360);
		bloomRenderTexture.Initialize(device.Get(), windowWidth, windowHeight);
	}

	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool Graphics::InitializeShaders()
{
	initComputeShader(&computeShader, device, L"ComputeShader.cso");

	D3D11_INPUT_ELEMENT_DESC layout6[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },

		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },

	};

	UINT numElements = ARRAYSIZE(layout6);
	//sizeof(*RtlpNumberOf(layout6));
	//OutputDebugStringA(("SIZEOF = "+std::to_string(sizeof(*RtlpNumberOf(layout6)))+"\n").c_str());

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"WEIGHTS", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BONE_IDs", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT,1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	numElements = ARRAYSIZE(layout);

	initPSShader(&materialPS, device, L"MaterialPS.cso");
	initPSShader(&material2PS, device, L"Material2PS.cso");
	initPSShader(&material3PS, device, L"Material3PS.cso");
	initVSShader(&deferredVS, device, L"DeferredVS.cso", layout, &numElements);
	initPSShader(&GbufferPS, device, L"GbufferPS.cso");
	initVSShader(&SSAOVS, device, L"SSAOVS.cso", layout, &numElements);
	initPSShader(&SSAOPS, device, L"SSAOPS.cso");
	initPSShader(&deferredPS, device, L"DeferredPS.cso");
	initPSShader(&pbrPS, device, L"pbrPS.cso");
	initPSShader(&transparentPbrPS, device, L"TransparentPbrPS.cso");
	initPSShader(&EnvironmentPS, device, L"EnvironmentPS.cso"); 
	initPSShader(&EnvironmentTransparentPS, device, L"EnvironmentTransparentPS.cso");
	initPSShader(&ParticlePS, device, L"ParticlesPS.cso");

	initVSShader(&ssrVS, device, L"ssrVS.cso", layout, &numElements);
	initPSShader(&ssrPS, device, L"ssrPS.cso");

	initVSShader(&SoftShadowsVS, device, L"SoftShadowVS.cso", layout, &numElements);

	initPSShader(&BloomBrightColorPS, device, L"BloomBrightColorPS.cso");


	initVSShader(&AnimatedVS, device, L"AnimatedVS.cso", layout, &numElements);
	initVSShader(&shadowAnimVS, device, L"ShadowAnimVS.cso", layout, &numElements);

	initVSShader(&depthAnimVS, device, L"DepthAnimVS.cso", layout, &numElements);

	initPSShader(&DepthAnimPS, device, L"DepthShaderAnimPS.cso");

	initPSShader(&ShadowAnimPS, device, L"ShadowAnimPS.cso");

	initVSShader(&vertexshader, device, L"vertexShaderEx.cso", layout, &numElements);

	initVSShader(&shadowVS, device, L"ShadowVS.cso", layout, &numElements);

	initPSShader(&pixelshader, device, L"pixelShaderEx.cso");

	initPSShader(&pixelshader_noLight, device, L"pixelshader_noLight.cso");
	initPSShader(&postProccessPS, device, L"PostProccessPS.cso");

	initPSShader(&SkyBloomPS, device, L"SkyBloomPS.cso");
	initPSShader(&BloomLightPS, device, L"BloomLightPS.cso");
	initPSShader(&cubeMapPS, device, L"CubemapPS.cso");
	initPSShader(&EquirectangularToCubemapPS, device, L"EquirectangularToCubemap.cso");

	initVSShader(&FPScameraVS, device, L"FPScameraVS.cso", layout, &numElements);

	initVSShader(&FPSanimatedVS, device, L"FPSanimatedVS.cso", layout, &numElements);


	initVSShader(&WaterVS, device, L"WaterVS.cso", layout, &numElements);

	initPSShader(&FPScameraPS, device, L"FPScameraPS.cso");
	initPSShader(&lightPS, device, L"lightPS.cso");
	initPSShader(&ShadowsShader, device, L"Shadows.cso");
	initPSShader(&IrradianceConvolutionPS, device, L"IrradianceConvolutionPS.cso");
	initPSShader(&PrifilterPS, device, L"PrifilterPS.cso");
	initPSShader(&BrdfPS, device, L"BrdfPS.cso");

	D3D11_INPUT_ELEMENT_DESC layout2[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(layout2);

	initVSShader(&SkyVS, device, L"SkyVS.cso", layout2, &numElements);
	initPSShader(&SkyPS, device, L"skyPS.cso");


	D3D11_INPUT_ELEMENT_DESC layout3[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};
	numElements = ARRAYSIZE(layout3);

	initVSShader(&m_shaderTextureVS, device, L"ShaderTextureVS.cso", layout3, &numElements);
	initPSShader(&m_shaderTexturePS, device, L"ShaderTexture.cso");
	initVSShader(&depthVertexShader, device, L"depthVertexShader.cso", layout3, &numElements);
	initPSShader(&depthPixelShader, device, L"depthPixelShader.cso");
	initVSShader(&horizontalBlurVS, device, L"HorizontalBlurVS.cso", layout3, &numElements);
	initPSShader(&horizontalBlurPS, device, L"horizontalBlurPS.cso");
	initVSShader(&verticalBlurVS, device, L"VerticalBlurVS.cso", layout3, &numElements);
	initPSShader(&verticalBlurPS, device, L"VerticalBlurPS.cso");
	initPSShader(&WaterPS, device, L"WaterPS.cso");
	initPSShader(&WaterBloomPS, device, L"WaterBloomPS.cso");
	initPSShader(&debugPS, device, L"DebugPS.cso");

	D3D11_INPUT_ELEMENT_DESC layout4[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	numElements = ARRAYSIZE(layout4);


	D3D11_INPUT_ELEMENT_DESC layout5[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },

	};
	numElements = ARRAYSIZE(layout5);


	return true;
}

bool Graphics::InitializeScene(Camera& camera)
{
	try
	{
		//Initialize constant buffers
		HRESULT hr = cb_vs_vertexshader.Initialize(device, deviceContext);
		//	COM_ERROR_IF_FAILED(hr, "Failed to create vertex Constant buffer.");
		hr = cb_vs_lightViewsVertexShader.Initialize(device, deviceContext);

		hr = cb_vs_shadowParams.Initialize(device, deviceContext);
		//	COM_ERROR_IF_FAILED(hr, "Failed to create vertex Constant buffer.");

		hr = cb_vs_blurWeights.Initialize(device, deviceContext);
		//	COM_ERROR_IF_FAILED(hr, "Failed to create vertex Constant buffer.");

		hr = cb_vs_reflection.Initialize(device, deviceContext);
		//	COM_ERROR_IF_FAILED(hr, "Failed to create vertex Constant buffer.");
		hr = cb_vs_cameraParams.Initialize(device, deviceContext);
		hr = cb_ps_camera.Initialize(device, deviceContext);
		hr = cb_ps_light.Initialize(device, deviceContext);


		hr = cb_ps_sky.Initialize(device, deviceContext);
		hr = cb_ps_water.Initialize(device, deviceContext);

		hr = cb_ps_PCF_Shadows.Initialize(device, deviceContext);

		hr = cb_cs_Forward_Plus.Initialize(device, deviceContext);
		hr = CB_CS_screen_to_view.Initialize(device, deviceContext);

		hr = cb_ps_Deferred_Shading.Initialize(device, deviceContext);

		hr = cb_ps_Pbr.Initialize(device, deviceContext);

		hr = cb_light_global.Initialize(device, deviceContext);

		hr = cb_ps_screenEffect.Initialize(device, deviceContext);

		if (SHADOWMAP_WIDTH == 1920 && SHADOWMAP_HEIGHT == 1080)
			this->cb_ps_PCF_Shadows.data.bias = 0.0003f;
		if (SHADOWMAP_WIDTH == 2048 && SHADOWMAP_HEIGHT == 2048)
			this->cb_ps_PCF_Shadows.data.bias = 0.0002f;
		this->cb_ps_PCF_Shadows.data.pcfLevel = 2;
		this->cb_ps_PCF_Shadows.data.enableShadows = true;
		//	COM_ERROR_IF_FAILED(hr, "Failed to create sky pixel Constant buffer.");
		this->cb_ps_PCF_Shadows.data.bias = 0.0002f;
		this->cb_ps_light.data.ambientLightColorAndStrength = XMFLOAT4(1.0f, 0.002f, 0.002f, 1.0f);
		cb_ps_Pbr.data.roughness = 0.0f;
		this->cb_ps_PCF_Shadows.data.bias = 0.00008f;
		cb_ps_screenEffect.data.gamma = 2.2f;
		cb_ps_screenEffect.data.bloomBrightness = 0.65f;
		cb_ps_screenEffect.data.bloomStrenth = 4.0f;

		cubeShape.InitCube(device, deviceContext, DirectX::XMMatrixIdentity(), cb_vs_vertexshader);
		cubeShapePBR.InitCube(device, deviceContext, DirectX::XMMatrixIdentity(), cb_vs_vertexshader);


		rect.InitRect(windowWidth, windowHeight, device, deviceContext, DirectX::XMMatrixIdentity(), cb_vs_vertexshader, false);
		rectSmall.InitRect(windowWidth/2, windowHeight/2, device, deviceContext, DirectX::XMMatrixIdentity(), cb_vs_vertexshader, false);
		fullScreenWidow.InitRect(windowWidth, windowHeight, device, deviceContext, DirectX::XMMatrixIdentity(), cb_vs_vertexshader, false);


		//shadowfullScreenWidow.InitRect(textureWidth, textureHeight, device, deviceContext, DirectX::XMMatrixIdentity(), cb_vs_vertexshader, true);


		//if (SHADOWMAP_WIDTH == 1024 && SHADOWMAP_HEIGHT == 1024)
		//{
		//	shadowfullScreenWidow.scale = DirectX::XMFLOAT3(5.175f, 5.840f, 1);
		//}
		//if (SHADOWMAP_WIDTH == 2048 && SHADOWMAP_HEIGHT == 2048)
		//{
		//	shadowfullScreenWidow.scale = DirectX::XMFLOAT3(10.250f, 11.700f, 1);
		//}
		//shadowfullScreenWidow.pos = DirectX::XMFLOAT3(0, 2.520, 0);

		if (!debugWindow.Initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader, false, 0, true))
		{
			return false;
		}
		camera.SetPosition(XMVECTOR{ 0,3,0 });


		debugWindow.GetGameObject()->rot = XMFLOAT3(1.570f, 3.375, 0.235);
		debugWindow.GetGameObject()->scale = XMFLOAT3(60.0f, 50.0, 1.0);
		camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);


		ViewMatrix2D = camera.GetViewMatrix();

		camera.PerspectiveFov(fov, static_cast<float>((windowWidth/2) / (windowHeight/2)) * 2, 0.1f, 10000.0f);


		ViewMatrix2DHalf = camera.GetViewMatrix();

		camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);


		probeMap.InitializeProbe(device.Get(), deviceContext.Get(), cb_vs_vertexshader);
		probeMap.SetupCamera(windowWidth, windowHeight);
		irradianceProbeMap.InitializeProbe(device.Get(), deviceContext.Get(), cb_vs_vertexshader);
		irradianceProbeMap.SetupCamera(windowWidth, windowHeight);

		this->deviceContext->VSSetConstantBuffers(1, 1, this->cb_vs_shadowParams.GetBuffer().GetAddressOf());

		this->cb_vs_shadowParams.data.shadowSize = downSampleWidth;
		this->cb_vs_shadowParams.data.padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->cb_vs_shadowParams.UpdateBuffer();

		this->deviceContext->VSSetConstantBuffers(2, 1, this->cb_vs_blurWeights.GetBuffer().GetAddressOf());
		this->cb_vs_blurWeights.data.weight1234 = blurWeight1234;
		this->cb_vs_blurWeights.data.weight5678 = blurWeight5678;
		this->cb_vs_blurWeights.data.weight9 = blurWeight9;
		this->cb_vs_blurWeights.data.padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
		
		this->cb_vs_blurWeights.UpdateBuffer();

		this->deviceContext->VSSetConstantBuffers(3, 1, this->cb_vs_cameraParams.GetBuffer().GetAddressOf());
		this->deviceContext->VSSetConstantBuffers(4, 1, this->cb_vs_lightViewsVertexShader.GetBuffer().GetAddressOf());
		this->deviceContext->VSSetConstantBuffers(5, 1, this->cb_light_global.GetBuffer().GetAddressOf());

		camera.SetPosition(0, 3, 0);



		noTextureAlbedo.CreateTextureDDS(device.Get(), deviceContext.Get(), "Data/Textures/albedo.dds");
		noTextureNormal.CreateTextureDDS(device.Get(), deviceContext.Get(), "Data/Textures/normal.dds");
		noTextureMetallic.CreateTextureDDS(device.Get(), deviceContext.Get(), "Data/Textures/metallic.dds");
		noTextureRoughness.CreateTextureDDS(device.Get(), deviceContext.Get(), "Data/Textures/roughness.dds");
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}



void Graphics::InitializeObjects(std::vector<GameObject*>& gameObjects,std::vector<PointLight>& pointLights,Camera& camera, std::vector< const char*>& objNames)
{



	if (!skydomeObject.Initialize("Data/Objects/skyDome.obj", device.Get(), deviceContext.Get(), cb_vs_vertexshader, false, false, false, false, false))
	{
		return;
	}
	skydomeObject.model.LoadModel("Data/Objects/skyDome.obj", false);

	crosshair.Initialize("Data/Particles/ParticleObj.obj", "Data/Particles/crosshair.png", device.Get(), deviceContext.Get(), cb_vs_vertexshader);
	muzzleFlash1.Initialize("Data/Particles/ParticleObj.obj", "Data/Objects/Pistol/handgun_Fire.png", device.Get(), deviceContext.Get(), cb_vs_vertexshader);
	muzzleFlash2.Initialize("Data/Particles/ParticleObj.obj", "Data/Objects/Pistol/handgun_Fire.png", device.Get(), deviceContext.Get(), cb_vs_vertexshader);

	LoadGeneralSettings();
	PreLoadSettings();
	LoadSkySettings(probeMap,centerColor,apexColor);



	for (int i = 0; i < ObjectsCount; ++i)
	{
		if (!CreateObject(gameObjects,ObjectsFilePaths[i], isAnimatedSave[i], isPBRSave[i], isTexturedSave[i], isEmptyTargetSave[i], isObjectDeleted[i]))
		{
			return;
		}
		gameObjects[i]->ID = i;

	}
	t.resize(gameObjects.size());
	createAsyncArr.resize(gameObjects.size());
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!gameObjects[i]->model.deleted)
		{
			if (gameObjects[i]->isTextured)
			{
				if (!gameObjects[i]->model.isInstancedObject)
				{
					gameObjects[i]->model.SetTextures(gameObjects[i]->isPBR);
					globalTextureStorage.push_back(gameObjects[i]->model._textures);
					globalTextureStorageObjFilePath.push_back(gameObjects[i]->filePath);
					
				}
			}
			else
			{
				Texture _tex[4] = { noTextureAlbedo,noTextureNormal,noTextureRoughness,noTextureMetallic };
				gameObjects[i]->model.SetTextures(gameObjects[i]->isPBR, _tex);
			}

			if (!gameObjects[i]->model.isInstancedObject)
			{
				//LoadModel(gameObjects[i]);
				t[i] = std::thread(&Graphics::LoadModel, this, gameObjects[i]);
			}
		}
	}
	OutputDebugStringA(("size = "+std::to_string(globalTextureStorageObjFilePath.size()) + "\n").c_str());
	
	
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		
		if (!gameObjects[i]->model.deleted)
		{
			if (gameObjects[i]->isTextured)
			{
				if (gameObjects[i]->model.isInstancedObject)
				{
					for (int j=0;j< globalTextureStorage.size();++j)
					{
						if (globalTextureStorageObjFilePath[j] == gameObjects[i]->filePath)
						{
							gameObjects[i]->model.SetTextures(gameObjects[i]->isPBR, globalTextureStorage[j]);
						}
					}
						//OutputDebugStringA((it->second + "\n").c_str());
						//OutputDebugStringA((gameObjects[i]->filePath + "\n").c_str());
						
				}
			}
			else
			{
				Texture _tex[4] = { noTextureAlbedo,noTextureNormal,noTextureRoughness,noTextureMetallic };
				gameObjects[i]->model.SetTextures(gameObjects[i]->isPBR, _tex);
			}
			if (gameObjects[i]->model.isInstancedObject)
			{
				if (!gameObjects[i]->model.texuresLoaded)
				{
					gameObjects[i]->model.SetTextures(gameObjects[i]->isPBR);
					globalTextureStorage.push_back(gameObjects[i]->model._textures);
					globalTextureStorageObjFilePath.push_back(gameObjects[i]->filePath);
					//Texture _tex[4] = { noTextureAlbedo,noTextureNormal,noTextureRoughness,noTextureMetallic };
					//gameObjects[i]->model.SetTextures(gameObjects[i]->isPBR, _tex);
				}
				//LoadModel(gameObjects[i]);
				t[i] = std::thread(&Graphics::LoadModel, this, gameObjects[i]);
			}
		}
		
	}
	for (int i = 0; i < t.size(); ++i)
	{
		if (t[i].joinable())
			t[i].join();
	}


	if (gameObjects.size() > 0)
	{
		frustums.resize(gameObjects.size());
		LoadSettings(gameObjects);

	}

	InitTestScene(camera);

	LoadGameObjectsData(gameObjects);



	LoadAnimations(gameObjects);

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		//if (gameObjects[i]->model.vertices.size() > 0 || gameObjects[i]->emptyTarget)
		//{
			objNames.push_back(gameObjects[i]->objectName.c_str());

		//}
	}

	
	pointLights.resize(lightsNR);
	cb_light_global.data.lightsSize = pointLights.size();
	this->cb_light_global.UpdateBuffer();
	

	///////////////////////////
	UpdateBuffers();
	UpdateShaders(pointLights,camera);
	


	for (int i = 0; i < pointLights.size(); ++i)
	{
		pointLights[i].Initialize(device.Get(), deviceContext.Get(), cb_vs_vertexshader);
		pointLights[i].lightname += std::to_string(i);

		pointLights[i].SetupCamera(windowWidth, windowHeight);

		lightNames.push_back(pointLights[i].lightname.c_str());

	}
	LoadLightSettings(pointLights);

	for (int j = 0; j < pointLights.size(); j++)
	{
		//if (pointLights[j].bShadow)
		//{
			//for (int i = 0; i < 6; ++i)
			//{
				//RenderTexture tempShadowTex;
				pointLights[j].m_shadowMap.Initialize(device.Get(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
				//pointLights[j].m_shadowMap.push_back(tempShadowTex);
			//}
		
		//}
	}

	//isObstacleSave.clear();
	//isWalkableSave.clear();
	//isAnimatedSave.clear();
	//isInstanceObjSave.clear();
	//isPBRSave.clear();
	//isEmptyTargetSave.clear();
	//isTexturedSave.clear();
	//isObjectDeleted.clear();
	//
	//isObstacleSave.~vector();
	//isWalkableSave.~vector();
	//isAnimatedSave.~vector();
	//isInstanceObjSave.~vector();
	//isPBRSave.~vector();
	//isEmptyTargetSave.~vector();
	//isTexturedSave.~vector();
	//isObjectDeleted.~vector();



}

void Graphics::FPSCounter()
{
	ImGuiIO& io = ImGui::GetIO();
	float fps = io.Framerate;
	////FPS COUNTER
	//static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	//fpsCounter += 1;
	//if (timer.GetMilisecondsElapsed() > 1000.0f)
	//{
	//	fpsString = "\n\n\nFPS: " + std::to_string(fps);
	//	
	//	fpsCounter = 0;
	//	timer.Restart();
	//}
	fpsString = "\n\n\nFPS: " + std::to_string(fps);
//Draw Text
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f);
	spriteBatch->End();
}



void Graphics::RenderFrame(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera, PhysXHandler& physXHandler, PhysXDebugDraw& physXDebugDraw, bool& HideUI, int& listbox_item_current, std::vector< const char*>& objNames, KeyboardClass& keyboard, MouseClass& mouse, float& mouseX, float& mouseY, int screenWidth, int screenHeight, bool& rightDown, int& selectedObject, GridClass& grid, NavMeshClass& navMesh)
{

	//this->deviceContext->OMSetBlendState(NULL, NULL, 1);

	//this->cb_ps_PCF_Shadows.UpdateBuffer();

	this->deviceContext->VSSetSamplers(1, 1, this->samplerState_Wrap.GetAddressOf());

	HRESULT hr;
	CD3D11_RASTERIZER_DESC shadowRasterizerDesc(D3D11_DEFAULT);

	shadowRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	shadowRasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	shadowRasterizerDesc.DepthBias = depthBias;
	shadowRasterizerDesc.DepthBiasClamp = clamp;
	shadowRasterizerDesc.SlopeScaledDepthBias = slopeBias;
	hr = this->device->CreateRasterizerState(&shadowRasterizerDesc, this->shadowRasterizerState.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");
	
	///////////////////////////
	UpdateBuffers();
	UpdateShaders(pointLights,camera);
	//////////////////////////
	
	AddObject(gameObjects, listbox_item_current, objNames);
	AddLight(pointLights);
	


	//#######################################################################################################################################


		//**************************RENDER SHADOWS***************************
	this->deviceContext->RSSetState(this->shadowRasterizerState.Get());

	if (this->cb_ps_PCF_Shadows.data.enableShadows)
	{
		RenderDynamicShadows(gameObjects, pointLights, camera);

	}



	this->deviceContext->RSSetState(this->rasterizerState.Get());
	//********************************************************************

	this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
	this->cb_vs_blurWeights.data.weight1234 = blurWeight1234;
	this->cb_vs_blurWeights.data.weight5678 = blurWeight5678;
	this->cb_vs_blurWeights.data.weight9 = blurWeight9;

	this->cb_vs_blurWeights.UpdateBuffer();


	deviceContext->RSSetViewports(1, &this->viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), rgb);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);


	PBRInit(gameObjects,camera);

	if (enablePostProccess)
	{
		RenderSceneToTexture(dt,gameObjects, pointLights, camera);
		//RenderLightsToTexture();
		bloomEffect(camera, bloomRenderTexture, SoftShadowsVS, BloomLightPS);
	}
	
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	if(switchView)
		RenderSceneToTexture(dt, gameObjects, pointLights, camera);

	deviceContext->RSSetViewports(1, &this->viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), rgb);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	 

	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	if(!enablePostProccess)
		ForwardRendering(dt, gameObjects, pointLights,camera);

	if (enablePostProccess)
	{
		//this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
		this->deviceContext->RSSetState(this->rasterizerState.Get());
		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->postProccessPS.GetShader(), NULL, 0);
		this->deviceContext->PSSetShaderResources(0, 1, &sceneTexture.shaderResourceView);
		this->deviceContext->PSSetShaderResources(1, 1, &bloomRenderTexture.shaderResourceView);
		//this->deviceContext->PSSetShaderResources(2, 1, &lightsRenderTexture.shaderResourceView);

		fullScreenWidow.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);

	}



	this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
	if (crosshair.particle.attachToCamera)
	{
		RenderObjects(ViewMatrix2D, camera.GetProjectionMatrix(), FPScameraVS, ParticlePS);
		camera.OrthographicFov(window_width, window_height, 0.0f, 1.0f);
		this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
	}
	else
	{
		RenderObjects(ViewMatrix2D, camera.GetProjectionMatrix(), vertexshader, ParticlePS);
		camera.OrthographicFov(window_width, window_height, 0.0f, 1.0f);
		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
	}




	deviceContext->PSSetShaderResources(0, 1, crosshair.texture.GetTextureResourceViewAddress());
	
	crosshair.particle.attachToCamera = true;
	crosshair.particle.scale = XMFLOAT3(15.0, 15.0, 15.0);
	crosshair.particle.rot = XMFLOAT3(0.0, 1.560, 0.0);
	crosshair.particle.pos = XMFLOAT3(0.0, 0.0f, 0.50);
	crosshair.particle.frustumEnable = false;
	//if (camera.PossessCharacter)
		crosshair.Draw(ViewMatrix2D, camera.GetProjectionMatrix(), true);


	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);


	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);



	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 10000.0f);
	//this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	//this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);

	
	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->debugPS.GetShader(), NULL, 0);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->isAnimated)
		{
			XMMATRIX worldMatrix = XMMatrixIdentity();
			gameObjects[i]->model.DrawBones(worldMatrix, camera.GetViewMatrix(), camera.GetProjectionMatrix(), false);
		}

	}
	const physx::PxRenderBuffer& rb = physXHandler.aScene->getRenderBuffer();
	for (physx::PxU32 i = 0; i < rb.getNbLines(); ++i)
	{
		const physx::PxDebugLine& line = rb.getLines()[i];
		physXDebugDraw.DebugDraw(device.Get(), deviceContext.Get(), &cb_vs_vertexshader, line, camera);
	}
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->lightPS.GetShader(), NULL, 0);

	this->deviceContext->RSSetState(this->rasterizerStateWireframe.Get());
	//navMesh.DrawBounds(camera);
	//navMesh.DrawNavMesh(camera);
	grid.DrawGrid(camera, navMesh.validNodes);

	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);
	this->deviceContext->PSSetShaderResources(0, 1, &brdfTexture.shaderResourceView);
	//probeMap.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix(), false);
	if (showDebugWindow)
	{
		

		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);
		this->deviceContext->PSSetShaderResources(0, 1, &brdfTexture.shaderResourceView);
		probeMap.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix(), false);
		irradianceProbeMap.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix(), false);

		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
		//this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);

		if (debugWindowMode == 0)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &PrifilterCubeTextures[0].shaderResourceView);
		}
		if (debugWindowMode == 1)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &PrifilterCubeTextures[1].shaderResourceView);
		}
		if (debugWindowMode == 2)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &PrifilterCubeTextures[2].shaderResourceView);
		}
		if (debugWindowMode == 3)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &PrifilterCubeTextures[3].shaderResourceView);
		}
		if (debugWindowMode == 4)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &PrifilterCubeTextures[4].shaderResourceView);
		}
		if (debugWindowMode == 5)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &PrifilterCubeTextures[5].shaderResourceView);
		}

		//this->deviceContext->RSSetState(this->rasterizerState.Get());
		this->deviceContext->PSSetShaderResources(0, 1, &bloomRenderTexture.shaderResourceView);
		rect.Draw(camera, depthStencilState2D.Get(), ViewMatrix2DHalf, camera.GetProjectionMatrix(), windowWidth, windowHeight);
		//debugWindow.Draw(camera, deviceContext.Get(), depthStencilState2D.Get(), ViewMatrix2D, fov, 512, 512, 1.0f, 1000.f);
		//smallWindow.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);

		this->deviceContext->VSSetShader(SoftShadowsVS.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->lightPS.GetShader(), NULL, 0);
		this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);

		cubeShape.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		if (debugWindowMode == 0)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &prefilterCubeMap[0].shaderResourceView);
		}
		if (debugWindowMode == 1)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &prefilterCubeMap[1].shaderResourceView);
		}
		if (debugWindowMode == 2)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &prefilterCubeMap[2].shaderResourceView);
		}
		if (debugWindowMode == 3)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &prefilterCubeMap[3].shaderResourceView);
		}
		if (debugWindowMode == 4)
		{
			this->deviceContext->PSSetShaderResources(0, 1, &prefilterCubeMap[4].shaderResourceView);
		}

		//this->deviceContext->PSSetShaderResources(0, 1, &prefilterCubeMap[0].shaderResourceView);
		this->deviceContext->PSSetShaderResources(0, 1, &irradianceConvCubeMap.shaderResourceView);
		this->deviceContext->PSSetShader(this->cubeMapPS.GetShader(), NULL, 0);
		cubeShape.DrawObject(camera, camera.GetViewMatrix(), false, true);

		//this->deviceContext->RSSetState(this->rasterizerState.Get());


		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);


		this->deviceContext->VSSetShader(SoftShadowsVS.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->lightPS.GetShader(), NULL, 0);


		//this->deviceContext->PSSetShaderResources(4, 1, &BloomFinalTexture.shaderResourceView);
		////sunlight.sunObject.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix(), false);
		////sunlight.Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix(), false);
		//

		this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);




		this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(this->pixelshader_noLight.GetShader(), NULL, 0);


		//this->deviceContext->PSSetShaderResources(0, 1, &blackAndWhiteTexture.shaderResourceView);


	}


	//**********************************************************
	//showDebugWindow = true;


	if (!HideUI)
	{
		RenderGui(gameObjects, pointLights,camera, keyboard, listbox_item_current, objNames, grid);
	}
	
	hr = this->swapchain->Present(vSync, NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Vsync: invalid value");
	}
}

void Graphics::SetDebugWindowMode(int debugWindowMode)
{
	this->debugWindowMode = debugWindowMode;
}


void Graphics::RenderGui(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights,Camera& camera, KeyboardClass& keyboard, int& listbox_item_current, std::vector< const char*>& objNames, GridClass& grid)
{

	this->FPSCounter();
	//Start ImGui
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	
	bool isInEditMode = false;

	static bool show_app_metrics = false;
	static bool show_app_console = false;
	static bool show_app_log = false;
	static bool show_app_style_editor = false;
	static bool show_lights = false;
	static bool show_objects = true;
	static bool show_particles = false;
	static bool show_general = false;

	if (show_app_metrics) { ImGui::ShowMetricsWindow(&show_app_metrics); }
	if (show_app_console) { ui.ShowExampleAppConsole(&show_app_console); }
	if (show_app_log) { ui.ShowExampleAppLog(&show_app_log); }
	if (show_app_style_editor) { ImGui::Begin("Style Editor", &show_app_style_editor); ImGui::ShowStyleEditor(); ImGui::End(); }

	bool open = false, save = false;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Engine"))
		{
			if (ImGui::MenuItem("Save", NULL))
				save = true;
			ImGui::MenuItem("Lights", NULL, &show_lights);
			ImGui::MenuItem("Objects", NULL, &show_objects);
			ImGui::MenuItem("Particles", NULL,&show_particles);
			ImGui::MenuItem("General", NULL,&show_general);
			

			ImGui::EndMenu();
		}
		ImGui::SameLine();
		if (ImGui::BeginMenu("Files"))
		{
			if (ImGui::MenuItem("Open", NULL))
				open = true;

			ImGui::EndMenu();
		}
		ImGui::SameLine();
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::DragFloat("FOV", &fov, 0.01, 0.0f);
			//ImGui::Checkbox("EnablePostProccess", &enablePostProccess);
			ImGui::DragInt("VSYNC", &vSync, 1.0f, 0, 3);
			ImGui::MenuItem("EnablePostProccess", NULL, &enablePostProccess);
			ImGui::MenuItem("showDebugWindow", NULL, &showDebugWindow);
			ImGui::MenuItem("Console", NULL, &show_app_console);
			ImGui::MenuItem("Log", NULL, &show_app_log);
			//ImGui::Checkbox("showDebugWindow", &showDebugWindow);
	
			ImGui::EndMenu();
		}
		ImGui::SameLine();

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
			ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);

			ImGui::EndMenu();
		}
		ImGui::SameLine();

		ImGui::NewLine();

		//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
		if (open)
			ImGui::OpenPopup("Open File");
		//if (save)
		//	ImGui::OpenPopup("Save File");

		/* Optional third parameter. Support opening only compressed rar/zip files.
		 * Opening any other file will show error, return false and won't close the dialog.
		 */
		if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(400, 200), "*.*,.obj,.dae,.gltf,.fbx,.glb"))
		{
			std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
			std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
			//f << file_dialog.selected_path;
			f = std::fstream(file_dialog.selected_path.c_str());
			if (f.good())
				inName = file_dialog.selected_path;

			OutputDebugStringA(("NAME = " + inName + "\n").c_str());
		}

		ImGui::Text(inName.c_str());

		ImGui::EndMainMenuBar();
	}



	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->isAttachedToObj)
		{
			if (gameObjects[i]->parentObj->model.deleted)
				gameObjects[i]->isAttachedToObj = false;
		}


		gameObjects[i]->ID = i;
		if (gameObjects[i]->isInEditMode)
		{
			modelViewer.EditModel(*gameObjects[i], camera);
			
			isInEditMode = true;
			break;
		}
	}
	if (!isInEditMode)
	{
		if (show_objects)
		{
			ImGui::Begin("Objects");

			ImGui::Checkbox("IsAnimated", &isAnim);
			ImGui::Checkbox("IsTextured", &isTex);
			ImGui::Checkbox("IsPBR", &isPbr);
			ImGui::Checkbox("emptyTarget", &emptyTarget);

			if (ImGui::Button("Add"))
			{

				if (f.good() || (inName.empty() && emptyTarget))
				{
					isWalkableSave.push_back(true);
					isObstacleSave.push_back(false);
					isInstanceObjSave.push_back(false);
					isAnimated.push_back(isAnim);
					isPBR.push_back(isPbr);
					isTextured.push_back(isTex);
					isEmptyTarget.push_back(emptyTarget);
					addObject = true;

				}
			}
			if (!f.good() && !inName.empty())
			{
				ImGui::Text("Invalid filepath!");
			}
			if (!emptyTarget && inName.empty())
			{
				ImGui::Text("Warning! Empty target.");
				ImGui::Text("Enable emptyTarget parameter to add an empty physics object.");
			}


			std::vector<const char*> treeNames;

			treeNames.reserve(objNames.size());
			for (int i = 0; i < objNames.size(); ++i)
			{
				if (!gameObjects[i]->isAttachedToObj)
				{
					if(!gameObjects[i]->model.deleted)
						treeNames.push_back(objNames[i]);
				}
			}

			//ImGuiWindowFlags wf = ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize;

			ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_SpanAvailWidth;

			static int _clicked = -1;



			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Close"))
						ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			// left
			static int selected = 0;
			ImGui::BeginChild("left pane", ImVec2(150, 0), true);
			for (int i = 0; i < treeNames.size(); ++i)
			{

				bool node_open = ImGui::TreeNode(treeNames[i]);
				bool childNode_open = false;

				if (node_open)
				{
					for (int j = 0; j < gameObjects.size(); j++)
					{
						if (gameObjects[j]->isAttachedToObj)
						{
							if (gameObjects[j]->parentObj->objectName == treeNames[i])
							{
								if (childNode_open = ImGui::TreeNode(gameObjects[j]->objectName.c_str()))
								{
									listbox_item_current = j;
									ImGui::TreePop();
								}
							}
						}
						else if (!childNode_open)
						{
							if (gameObjects[j]->objectName == treeNames[i])
							{
								listbox_item_current = j;
							}
						}
					}
					ImGui::TreePop();

				}

			}
			//}
			ImGui::EndChild();
			ImGui::SameLine();



			//ImGui::ListBox("Objects", &listbox_item_current, objNames.data(), objNames.size(), 15);
		}

		}
		

	ImGui::NewLine();
	//grid.gridBoundsShape.DrawGui("grid");
	if (ImGui::Button("bCreateMesh"))
	{
		grid.bCreateMesh = true;
	}
	ImGui::Checkbox("ShowGrid", &grid.showGrid);
	if (canCopy)
	{
		if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('C'))
		{

			canCopy = false;
			copyObject = true;
		}
	}

	if (!keyboard.KeyIsPressed(VK_CONTROL) && !keyboard.KeyIsPressed('C'))
	{
		canCopy = true;
	}
	if (dataCopied && canPaste)
	{
		if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('V'))
		{
			canPaste = false;
			copyObject = false;
			pasteObject = true;
		}
	}
	if (!keyboard.KeyIsPressed(VK_CONTROL) && !keyboard.KeyIsPressed('V'))
	{
		canPaste = true;
	}

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!isInEditMode)
		{
			if (!gameObjects[i]->model.deleted)
			{
				if (objNames[listbox_item_current] == gameObjects[i]->objectName.c_str())
				{
					gameObjects[i]->listBoxSelected = true;
				}
				else
				{
					gameObjects[i]->listBoxSelected = false;
				}
				if (gameObjects[i]->objectSelected || objNames[listbox_item_current] == gameObjects[i]->objectName.c_str())
				{
					if (copyObject)
					{
						copiedObjectData.FilePath = gameObjects[i]->filePath;
						copiedObjectData.isTransparent = gameObjects[i]->isTransparent;
						copiedObjectData.isAnimated = gameObjects[i]->isAnimated;
						copiedObjectData.isAi = gameObjects[i]->isAI;
						copiedObjectData.attachToCamera = gameObjects[i]->attachToCamera;
						copiedObjectData.frustumEnable = gameObjects[i]->frustumEnable;
						copiedObjectData.isPBR = gameObjects[i]->isPBR;
						copiedObjectData.isTextured = gameObjects[i]->isTextured;

						copiedObjectData.mass = gameObjects[i]->phsxObj.mass;
						copiedObjectData.collisionScale = gameObjects[i]->physxScale;
						copiedObjectData.radius = gameObjects[i]->phsxObj.radius;
						copiedObjectData.objType = gameObjects[i]->phsxObj.physXShapeType;
						copiedObjectData.convexMeshDetail = gameObjects[i]->phsxObj.convexMeshDetail;
						copiedObjectData.triangleMeshStride = gameObjects[i]->phsxObj.triangleMeshStride;
						if (gameObjects[i]->isTextured)
						{
							copiedObjectData.tex[0] = gameObjects[i]->model._textures[0];
							copiedObjectData.tex[1] = gameObjects[i]->model._textures[1];
							copiedObjectData.tex[2] = gameObjects[i]->model._textures[2];
							copiedObjectData.tex[3] = gameObjects[i]->model._textures[3];

						}

						copiedObjectData.pos = gameObjects[i]->pos;
						copiedObjectData.scale = gameObjects[i]->scale;
						copiedObjectData.rot = gameObjects[i]->rot;
						copiedObjectData.translateModel = gameObjects[i]->translateModel;

						copiedObjectData.frustumScale = gameObjects[i]->frustumScale;

						copiedObjectData.isCharacter = gameObjects[i]->phsxObj.isCharacter;
						copiedObjectData.isWalkable = gameObjects[i]->isWalkable;
						copiedObjectData.isObstacle = gameObjects[i]->isObstacle;

						if (copiedObjectData.AnimFilePaths.size() > 0)
						{
							copiedObjectData.AnimFilePaths.clear();
						}
						if (gameObjects[i]->isAnimated && gameObjects[i]->AnimFiles.size() > 0)
						{
							for (int j = 0; j < gameObjects[i]->AnimFiles.size(); ++j)
							{
								copiedObjectData.AnimFilePaths.push_back(gameObjects[i]->AnimFiles[j]);
							}
						}
						OutputDebugStringA("Data copied!!!!\n\n");

						for (int j = 0; j < copiedObjectData.AnimFilePaths.size(); ++j)
						{
							OutputDebugStringA((copiedObjectData.AnimFilePaths[j] + "\n").c_str());
						}

						copyObject = false;
						dataCopied = true;
					}
					gameObjects[i]->DrawGui("GameObject" + std::to_string(i), deviceContext.Get(), keyboard, camera);
				}

			}
			else
			{
				if (gameObjects[i]->model.vertices.size() > 0)
				{
					//gameObjects[i]->objectName = "<empty>";
					gameObjects[i]->listBoxSelected = false;
				}
			}
		}
		else
		{
			ImGui::Begin("Object");
			if (gameObjects[i]->isInEditMode)
			{
				ImGui::ListBox("Objects", &gameObjects[i]->list_box_selected_obj, objNames.data(), objNames.size(), 15);
				gameObjects[i]->DrawGui("GameObject" + std::to_string(i), deviceContext.Get(), keyboard, camera);
			}
				
		}
		
		
		
	}


	if (!isInEditMode)
	{
		if (show_lights)
		{
			ImGui::Begin("Lights");

			ImGui::InputDouble("Bias", &this->cb_ps_PCF_Shadows.data.bias);
			ImGui::DragInt("DepthBias", &depthBias);
			ImGui::DragFloat("Clamp", &clamp, 0.01f);
			ImGui::InputDouble("SlopeBias", &slopeBias);
			ImGui::DragFloat4("blurWeight1234", &this->blurWeight1234.x, 0.01f, 0.0f);
			ImGui::DragFloat4("blurWeight5678", &this->blurWeight5678.x, 0.01f, 0.0f);
			ImGui::DragFloat("blurWeight9", &this->blurWeight9, 0.01f, 0.0f);
			ImGui::DragFloat("Gamma", &this->cb_ps_screenEffect.data.gamma, 0.1f, 0.0f);
			ImGui::DragFloat("BloomBrightness", &this->cb_ps_screenEffect.data.bloomBrightness, 0.0001f, 0.0f);
			ImGui::DragFloat("bloomStrenth", &this->cb_ps_screenEffect.data.bloomStrenth, 0.001f, 0.0f);
			if (ImGui::Button("AddLight"))
			{
				addLight = true;
			}

			ImGui::DragFloat("ShadowCullingDistance", &this->shadowCullingRadius, 0.5f, 0.0f);
			ImGui::Checkbox("Enable shadows", &this->cb_ps_PCF_Shadows.data.enableShadows);
			ImGui::ListBox("Lights", &listbox_light_current, lightNames.data(), lightNames.size(), 15);


			ImGui::NewLine();
			ImGui::NewLine();
			ImGui::NewLine();

			for (int i = 0; i < pointLights.size(); ++i)
			{
				if (lightNames[listbox_light_current] == pointLights[i].lightname)
				{
					pointLights[i].DrawGui("Light");
				}
			}



			ImGui::DragFloat3("AmbientLightColor", &this->cb_ps_light.data.ambientLightColorAndStrength.x, 0.005f, 0.0f, 1.0f);
			ImGui::DragFloat("AmbientStrength", &this->cb_ps_light.data.ambientLightColorAndStrength.w, 0.005f, -0.2f, 10.0f);

		}
		
		if (show_general)
		{
			ImGui::Begin("D3D11_Engine");
			ImGui::Text(("X: " + std::to_string(camera.pos.x) + " |Y: " + std::to_string(camera.pos.x) + " |Z: " + std::to_string(camera.pos.z)).c_str());

			
			ImGui::DragFloat("FOV", &fov, 0.01, 0.0f);
			ImGui::Checkbox("EnablePostProccess", &enablePostProccess);
			ImGui::DragInt("VSYNC", &vSync, 1.0f, 0, 3);
			ImGui::Checkbox("showDebugWindow", &showDebugWindow);
			fullScreenWidow.DrawGui("fullScreenWidow");
			rect.DrawGui("rect");

			//ImGui::Begin("Environment probes");

			if (ImGui::CollapsingHeader("Sky/probeMap"))
			{
				if (ImGui::Button("Recalculate"))
				{
					recalculateProbeMaps = true;
				}
				probeMap.DrawGui("probeMap");
				irradianceProbeMap.DrawGui("irradianceProbeMap");
				if (ImGui::CollapsingHeader("Sky"))
				{
					//skydomeObject.DrawGui("skydome", deviceContext.Get(), keyboard,camera);
					ImGui::DragFloat3("SkyRot", &this->skydomeObject.rot.x, 0.005f);
					ImGui::DragFloat4("ApexColor", &this->apexColor.x, 0.005f);
					ImGui::DragFloat4("CenterColor", &this->centerColor.x, 0.005f);
				}

			}

		}
		
		if (show_particles)
		{
			ImGui::Begin("Particles");
			crosshair.particle.DrawGui("crosshair", deviceContext.Get(), keyboard, camera);
			muzzleFlash1.particle.DrawGui("muzzleFlash1", deviceContext.Get(), keyboard, camera);
			muzzleFlash2.particle.DrawGui("muzzleFlash2", deviceContext.Get(), keyboard, camera);
		}


	}

	if (save)
		SaveGameObjectsData(gameObjects, pointLights, probeMap, centerColor, apexColor);

	//ImGui::ShowDemoWindow();
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}


bool Graphics::CreateObject(std::vector<GameObject*>& gameObjects,std::string fileName, bool isAnimated, bool isPBR, bool isTextured, bool isEmptyTarget, bool isObjectDeleted)
{

	gameObjects.push_back(new GameObject());
	gameObjects[gameObjects.size() - 1]->isWalkable = isWalkableSave[gameObjects.size() - 1];
	gameObjects[gameObjects.size() - 1]->isObstacle = isObstacleSave[gameObjects.size() - 1];
	gameObjects[gameObjects.size() - 1]->model.isInstancedObject = isInstanceObjSave[gameObjects.size() - 1];
	gameObjects[gameObjects.size() - 1]->emptyTarget = isEmptyTarget;
	gameObjects[gameObjects.size() - 1]->isAnimated = isAnimated;
	gameObjects[gameObjects.size() - 1]->isPBR = isPBR;
	gameObjects[gameObjects.size() - 1]->isTextured = isTextured;
	if (!gameObjects[gameObjects.size() - 1]->Initialize(fileName, device.Get(), deviceContext.Get(), cb_vs_vertexshader, isAnimated, isPBR, isTextured, isEmptyTarget, isObjectDeleted))
	{
		ErrorLogger::Log("Failed to load game object.");

		return false;
	}

	return true;
}

void Graphics::AddObject(std::vector<GameObject*>& gameObjects, int& listbox_item_current, std::vector< const char*>& objNames)
{
	if (addObject)
	{
		CreateObject(gameObjects, inName, isAnimated[isAnimated.size() - 1], isPBR[isPBR.size() - 1], isTextured[isTextured.size() - 1], isEmptyTarget[isEmptyTarget.size() - 1], false);
		//LoadModel(gameObjects[gameObjects.size() - 1]);
		if(gameObjects[gameObjects.size() - 1]->isTextured)
			gameObjects[gameObjects.size() - 1]->model.SetTextures(gameObjects[gameObjects.size() - 1]->isPBR);
		addObjAsync = std::async(std::launch::async, &Graphics::LoadModel, this, gameObjects[gameObjects.size() - 1]);


		frustums.resize(gameObjects.size());

		objNames.push_back(gameObjects[gameObjects.size() - 1]->objectName.c_str());
		listbox_item_current = objNames.size() - 1;
		addObject = false;
		
	}

	if (pasteObject)
	{
		pasteObject = false;
		isInstanceObjSave.push_back(true);
		CreateObject(gameObjects, copiedObjectData.FilePath, copiedObjectData.isAnimated, copiedObjectData.isPBR, copiedObjectData.isTextured, copiedObjectData.isEmptyTarget, false);
		gameObjects[gameObjects.size() - 1]->model.isInstancedObject = true;
		gameObjects[gameObjects.size() - 1]->constructShape = true;
		gameObjects[gameObjects.size() - 1]->isTransparent = copiedObjectData.isTransparent;
		gameObjects[gameObjects.size() - 1]->isAI = copiedObjectData.isAi;
		gameObjects[gameObjects.size() - 1]->attachToCamera = copiedObjectData.attachToCamera;
		gameObjects[gameObjects.size() - 1]->emptyTarget = copiedObjectData.isEmptyTarget;
		gameObjects[gameObjects.size() - 1]->isWalkable = copiedObjectData.isWalkable;
		gameObjects[gameObjects.size() - 1]->isObstacle = copiedObjectData.isObstacle;

		gameObjects[gameObjects.size() - 1]->pos = copiedObjectData.pos;
		gameObjects[gameObjects.size() - 1]->scale = copiedObjectData.scale;
		gameObjects[gameObjects.size() - 1]->rot = copiedObjectData.rot;
		gameObjects[gameObjects.size() - 1]->translateModel = copiedObjectData.translateModel;

		gameObjects[gameObjects.size() - 1]->frustumEnable = copiedObjectData.frustumEnable;

		gameObjects[gameObjects.size() - 1]->frustumScale = copiedObjectData.frustumScale;

		gameObjects[gameObjects.size() - 1]->phsxObj.mass = copiedObjectData.mass;
		gameObjects[gameObjects.size() - 1]->physxScale = copiedObjectData.collisionScale;
		gameObjects[gameObjects.size() - 1]->radius = copiedObjectData.radius;

		gameObjects[gameObjects.size() - 1]->phsxObj.physXShapeType = copiedObjectData.objType;
		gameObjects[gameObjects.size() - 1]->phsxObj.convexMeshDetail = copiedObjectData.convexMeshDetail;
		gameObjects[gameObjects.size() - 1]->phsxObj.triangleMeshStride = copiedObjectData.triangleMeshStride;

		gameObjects[gameObjects.size() - 1]->isAnimated = copiedObjectData.isAnimated;
		gameObjects[gameObjects.size() - 1]->phsxObj.isCharacter = copiedObjectData.isCharacter;

		if (gameObjects[gameObjects.size() - 1]->isTextured)
		{
			gameObjects[gameObjects.size() - 1]->model.SetTextures(gameObjects[gameObjects.size() - 1]->isPBR,copiedObjectData.tex);
		}
	
		if (gameObjects[gameObjects.size() - 1]->isAnimated)
		{
			LoadModel(gameObjects[gameObjects.size() - 1]);
		}
		else
		{
			addObjAsync = std::async(std::launch::async, &Graphics::LoadModel, this, gameObjects[gameObjects.size() - 1]);
		}
	


		frustums.resize(gameObjects.size());

		objNames.push_back(gameObjects[gameObjects.size() - 1]->objectName.c_str());
		listbox_item_current = objNames.size() - 1;

		if (gameObjects[gameObjects.size() - 1]->isAnimated)
		{
			for (int i = 0; i < copiedObjectData.AnimFilePaths.size(); ++i)
			{
				//OutputDebugStringA("LOADING ANIMATION....\n");
				gameObjects[gameObjects.size() - 1]->AnimCount++;
				//createAsyncArr[i] = std::async(std::launch::async, &ModelLoader::LoadModel, gameObjects[gameObjects.size() - 1]->model, copiedObjectData.AnimFilePaths[i], copiedObjectData.isTextured);
				gameObjects[gameObjects.size() - 1]->model.LoadModel(copiedObjectData.AnimFilePaths[i], copiedObjectData.isTextured);
				gameObjects[gameObjects.size() - 1]->AnimFiles.push_back(copiedObjectData.AnimFilePaths[i]);
			}
		}

		OutputDebugStringA("Data pasted!!!!\n");
		//copiedObjectData.AnimFilePaths.clear();
	}
}

void Graphics::AddLight(std::vector<PointLight>& pointLights)
{
	if (addLight)
	{
		CreateLight(pointLights,device.Get(), deviceContext.Get(), cb_vs_vertexshader);
		addLight = false;
	}

}

void Graphics::CreateLight(std::vector<PointLight>& pointLights, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{

	cb_light_global.data.lightsSize++;
	cb_light_global.UpdateBuffer();

	pointLights.push_back(PointLight());
	pointLights[pointLights.size() - 1].Initialize(device, deviceContext, cb_vs_vertexshader);
	pointLights[pointLights.size() - 1].lightname += std::to_string(pointLights.size() - 1);
	pointLights[pointLights.size() - 1].SetupCamera(windowWidth, windowHeight);




	lightNames.clear();
	for (int i = 0; i < pointLights.size(); ++i)
	{
		lightNames.push_back(pointLights[i].lightname.c_str());
	}


	listbox_light_current = lightNames.size() - 1;
	//RenderTexture tempShadowTex;
	pointLights[pointLights.size()-1].m_shadowMap.Initialize(device,SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	//shadowMap[0].push_back(tempShadowTex);
	addLight = false;
}


void Graphics::LoadModel(GameObject* gameObject)
{
	if (!gameObject->emptyTarget)
	{
		gameObject->model.LoadModel(gameObject->model._filepath, gameObject->isTextured);
		//OutputDebugStringA("FALSE!!!!\n");
	}

	else
	{
		//OutputDebugStringA("TRUE!!!!\n");
	}
}
void Graphics::LoadAnimation(GameObject* gameObject,std::string filepath)
{
	if (!gameObject->emptyTarget)
	{
		gameObject->model.LoadModel(filepath, gameObject->isTextured);
		//OutputDebugStringA("FALSE!!!!\n");
	}

	else
	{
		//OutputDebugStringA("TRUE!!!!\n");
	}
}

void Graphics::LoadAnimations(std::vector<GameObject*>& gameObjects)
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->model.isAnimated)
		{
			if (gameObjects[i]->AnimCount > 0 && gameObjects[i]->AnimFiles.size() > 0 && !gameObjects[i]->model.deleted)
			{
				for (int j = 0; j < gameObjects[i]->AnimFiles.size(); ++j)
				{
					//OutputDebugStringA("LOADING ANIMATION....\n");
					if (gameObjects[i]->model.bonesLoaded)
					{
						//createAsyncArr[i] = std::async(std::launch::async, &Graphics::LoadAnimation, this, gameObjects[i], gameObjects[i]->AnimFiles[j]);
						t[i] = std::thread(&Graphics::LoadAnimation, this, gameObjects[i], gameObjects[i]->AnimFiles[j]);
						//LoadAnimation(gameObjects[i], gameObjects[i]->AnimFiles[j]);
					}
				}
			}
		}
	}
	for (int i = 0; i < t.size(); ++i)
	{
		if (t[i].joinable())
		{
			t[i].join();
		}
	}
}




bool Graphics::initPSShader(PixelShader* pixelShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath)
{
	if (!pixelShader->Initialize(device, filePath))
	{
		return false;
	}
	return true;
}

bool Graphics::initVSShader(VertexShader* vertexShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT* numElements)
{
	if (!vertexShader->Initialize(device, filePath, layoutDesc, *numElements))
	{
		return false;
	}
	return true;
}

bool Graphics::initComputeShader(ComputeShader* computeShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath)
{
	if (!computeShader->Initialize(device, filePath))
	{
		return false;
	}
	return true;
}

void Graphics::CreateUAV(ID3D11UnorderedAccessView* UAV, ID3D11Texture2D* m_renderTargetTexture)
{
	HRESULT hr;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
	UAVdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVdesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	UAVdesc.Texture2D.MipSlice = 0;
	hr = device->CreateUnorderedAccessView(m_renderTargetTexture, &UAVdesc, &UAV);
	COM_ERROR_IF_FAILED(hr, "Failed to UAV");
}

float Graphics::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void Graphics::PBRInit(std::vector<GameObject*>& gameObjects,Camera& camera)
{
	irradianceProbeMap.UpdateCamera(windowWidth, windowHeight);
	if (!TextureInit)
	{

		for (int i = 0; i < 3; ++i)
		{
			cubeShapePBR.pos = irradianceProbeMap.pos;
			cubeShapePBR.scale = XMFLOAT3(1, 1, 1);

			for (int i = 0; i < 6; ++i)
			{
				deviceContext->RSSetViewports(1, &CubeTextures[i].m_viewport);
				cubeMapRender(gameObjects, camera, CubeTextures[i], i, SoftShadowsVS, EnvironmentPS);
			}

			cubeTex.push_back(CubeTextures[1].m_renderTargetTexture);
			cubeTex.push_back(CubeTextures[0].m_renderTargetTexture);
			cubeTex.push_back(CubeTextures[2].m_renderTargetTexture);
			cubeTex.push_back(CubeTextures[3].m_renderTargetTexture);
			cubeTex.push_back(CubeTextures[5].m_renderTargetTexture);
			cubeTex.push_back(CubeTextures[4].m_renderTargetTexture);

			//if (TextureInit2)
			environmentCubeMap.CubeMapTexture(device.Get(), deviceContext.Get(), cubeTex);
			cubeTex.clear();
		
				for (int i = 0; i < 6; ++i)
				{
					deviceContext->RSSetViewports(1, &IrradianceConvCubeTextures[i].m_viewport);
					IrradianceConvolutionRender(camera, IrradianceConvCubeTextures[i], i, SoftShadowsVS, IrradianceConvolutionPS);
				}

				cubeTex.push_back(IrradianceConvCubeTextures[1].m_renderTargetTexture);
				cubeTex.push_back(IrradianceConvCubeTextures[0].m_renderTargetTexture);
				cubeTex.push_back(IrradianceConvCubeTextures[2].m_renderTargetTexture);
				cubeTex.push_back(IrradianceConvCubeTextures[3].m_renderTargetTexture);
				cubeTex.push_back(IrradianceConvCubeTextures[5].m_renderTargetTexture);
				cubeTex.push_back(IrradianceConvCubeTextures[4].m_renderTargetTexture);

				irradianceConvCubeMap.CubeMapTexture(device.Get(), deviceContext.Get(), cubeTex);

				cubeTex.clear();

				probeMap.UpdateCamera(windowWidth, windowHeight);



				unsigned int maxMipLevels = 5;
				for (int mip = 0; mip < maxMipLevels; ++mip)
				{
					//unsigned int mipWidth = 128 * std::pow(0.5, mip);
					//unsigned int mipHeight = 128 * std::pow(0.5, mip);
					cb_ps_Pbr.data.roughness = mip * 0.05f;
					//float roughness = (float)mip / (float)(maxMipLevels - 1);
					//cb_ps_Pbr.data.roughness = roughness;
					cb_ps_Pbr.UpdateBuffer();

					for (int i = 0; i < 6; ++i)
					{
						PrifilterCubeTextures[i].Initialize(device.Get(), 512 / (mip + 1), 512 / (mip + 1));

						deviceContext->RSSetViewports(1, &PrifilterCubeTextures[i].m_viewport);
						IrradianceConvolutionRender(camera, PrifilterCubeTextures[i], i, SoftShadowsVS, PrifilterPS);
					}


					cubeTex.push_back(PrifilterCubeTextures[1].m_renderTargetTexture);
					cubeTex.push_back(PrifilterCubeTextures[0].m_renderTargetTexture);
					cubeTex.push_back(PrifilterCubeTextures[2].m_renderTargetTexture);
					cubeTex.push_back(PrifilterCubeTextures[3].m_renderTargetTexture);
					cubeTex.push_back(PrifilterCubeTextures[5].m_renderTargetTexture);
					cubeTex.push_back(PrifilterCubeTextures[4].m_renderTargetTexture);

					prefilterCubeMap[mip].CubeMapTexture(device.Get(), deviceContext.Get(), cubeTex, 512 / (mip + 1), 512 / (mip + 1), 1);
					//prefilterCubeMap[mip].CubeMapTexture(device.Get(), deviceContext.Get(), cubeTex);
					cubeTex.clear();

				}

				deviceContext->RSSetViewports(1, &brdfTexture.m_viewport);

				BrdfRender(camera, brdfTexture, SoftShadowsVS, BrdfPS);

				TextureInit = true;
		}

	}

	if (recalculateProbeMaps)
	{
		TextureInit = false;
		recalculateProbeMaps = false;
	}

}
void Graphics::CreateShadowMap()
{
}


void Graphics::RenderObjects(XMMATRIX& viewMatrix, XMMATRIX& ProjectionMatrix, VertexShader& vertexShader, PixelShader& pixelShader)
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
void Graphics::UpdateShaders(std::vector<PointLight>& pointLights, Camera& camera)
{
	cb_light_global.data.lightsSize = pointLights.size();
	this->cb_light_global.UpdateBuffer();
	this->cb_vs_vertexshader.UpdateBuffer();

	//this->cb_vs_watershader.data.sunPos.x = sunlight.pos.x;
	//this->cb_vs_watershader.data.sunPos.y = sunlight.pos.y;
	//this->cb_vs_watershader.data.sunPos.z = sunlight.pos.z;
	//this->cb_vs_watershader.data.sunPos.w = 1;

	//this->cb_vs_watershader.data.cameraPos.x = camera.pos.x;
	//this->cb_vs_watershader.data.cameraPos.y = camera.pos.y;
	//this->cb_vs_watershader.data.cameraPos.z = camera.pos.z;
	//this->cb_vs_watershader.data.cameraPos.w = 1;

	this->cb_vs_cameraParams.data.cameraPos.x = camera.GetPositionFloat3().x;
	this->cb_vs_cameraParams.data.cameraPos.y = camera.GetPositionFloat3().y;
	this->cb_vs_cameraParams.data.cameraPos.z = camera.GetPositionFloat3().z;
	this->cb_vs_cameraParams.data.cameraPos.w = 1;

	this->cb_vs_cameraParams.UpdateBuffer();

	float constant = 1.0f;
	float linear = 0.7f;
	float quadratic = 1.8f;



	for (int i = 0; i < pointLights.size(); ++i)
	{
		//float radius = pointLights[i].radius;
		//float radius = (-linear + std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * pointLights[i].radius))) / (2 * quadratic) ;

		this->cb_ps_Deferred_Shading.data.Radius1[i].x = pointLights[i].radius;
		this->cb_ps_Deferred_Shading.data.Radius1[i].y = pointLights[i].radius;
		this->cb_ps_Deferred_Shading.data.Radius1[i].z = pointLights[i].radius;
		this->cb_ps_Deferred_Shading.data.Radius1[i].w = pointLights[i].radius;

		this->cb_ps_Deferred_Shading.data.cutOff[i].x = pointLights[i].cutOff;
		this->cb_ps_Deferred_Shading.data.cutOff[i].y = pointLights[i].cutOff;
		this->cb_ps_Deferred_Shading.data.cutOff[i].z = pointLights[i].cutOff;
		this->cb_ps_Deferred_Shading.data.cutOff[i].w = pointLights[i].cutOff;

		this->cb_ps_light.data.dynamicLightPosition[i].x = pointLights[i].pos.x;
		this->cb_ps_light.data.dynamicLightPosition[i].y = pointLights[i].pos.y;
		this->cb_ps_light.data.dynamicLightPosition[i].z = pointLights[i].pos.z;

		this->cb_ps_light.data.dynamicLightColor[i].x = pointLights[i].lightColor.x;
		this->cb_ps_light.data.dynamicLightColor[i].y = pointLights[i].lightColor.y;
		this->cb_ps_light.data.dynamicLightColor[i].z = pointLights[i].lightColor.z;

		this->cb_ps_light.data.dynamicLightAttenuationAndStrength[i].x = pointLights[i].attenuation_a;
		this->cb_ps_light.data.dynamicLightAttenuationAndStrength[i].y = pointLights[i].attenuation_b;
		this->cb_ps_light.data.dynamicLightAttenuationAndStrength[i].z = pointLights[i].attenuation_c;
		this->cb_ps_light.data.dynamicLightAttenuationAndStrength[i].w = pointLights[i].lightAttenuation;

		this->cb_ps_light.data.dynamicLightColor[i].w = pointLights[i].lightStrenth;


		this->cb_ps_light.data.lightDirectionAndSpecularPower[i].x = pointLights[i].direction.x;
		this->cb_ps_light.data.lightDirectionAndSpecularPower[i].y = pointLights[i].direction.y;
		this->cb_ps_light.data.lightDirectionAndSpecularPower[i].z = pointLights[i].direction.z;




		this->cb_ps_light.data.lightDirectionAndSpecularPower[i].w = pointLights[i].specularPower;

		this->cb_ps_light.data.specularColor[i].x = pointLights[i].specularColor.x;
		this->cb_ps_light.data.specularColor[i].y = pointLights[i].specularColor.y;
		this->cb_ps_light.data.specularColor[i].z = pointLights[i].specularColor.z;



	}

	this->cb_ps_light.data.enableLights = enableLights;
	this->cb_ps_light.data.cameraPos = XMFLOAT3(camera.GetPositionFloat3().x, camera.GetPositionFloat3().y, camera.GetPositionFloat3().z);

	this->cb_ps_light.data.sunLightPosition.x = 0;
	this->cb_ps_light.data.sunLightPosition.y = 0;
	this->cb_ps_light.data.sunLightPosition.z = 0;

	this->cb_ps_light.data.sunLightColor.x = 0;
	this->cb_ps_light.data.sunLightColor.y = 0;
	this->cb_ps_light.data.sunLightColor.z = 0;

	this->cb_ps_light.data.sunLightColor.w = 0;


	this->cb_ps_light.data.sunlightDirectionAndSpecularPower.x = 0;
	this->cb_ps_light.data.sunlightDirectionAndSpecularPower.y = 0;
	this->cb_ps_light.data.sunlightDirectionAndSpecularPower.z = 0;

	this->cb_ps_light.data.sunlightDirectionAndSpecularPower.w = 0;

	this->cb_ps_light.data.sunspecularColor.x = 0;
	this->cb_ps_light.data.sunspecularColor.y = 0;
	this->cb_ps_light.data.sunspecularColor.z = 0;


	this->cb_ps_light.data.lightsNR = 1;
	this->cb_ps_light.data.padding = XMFLOAT3(0.0f, 0.0f, 0.0f);


	for (int i = 0; i < pointLights.size(); ++i)
	{
		this->cb_ps_light.data.enabledLightsAttribs[i].x = pointLights[i].isLightEnabled;
		this->cb_ps_light.data.enabledLightsAttribs[i].y = pointLights[i].bShadow;
		this->cb_ps_light.data.enabledLightsAttribs[i].z = pointLights[i].lightType;
	}

	cb_ps_camera.data.projectionMatrix = camera.GetProjectionMatrix();
	cb_ps_camera.data.viewMatrix = camera.GetViewMatrix();

	this->cb_ps_light.UpdateBuffer();
	this->cb_ps_PCF_Shadows.UpdateBuffer();
	this->cb_ps_Deferred_Shading.UpdateBuffer();
	cb_ps_camera.UpdateBuffer();
	this->cb_ps_Pbr.UpdateBuffer();

	CB_CS_screen_to_view.data.InverseProjection = DirectX::XMMatrixInverse(nullptr, camera.GetProjectionMatrix());
	CB_CS_screen_to_view.data.ScreenDimensions = XMFLOAT2(1600, 900);
	CB_CS_screen_to_view.UpdateBuffer();

	cb_ps_sky.data.apexColor = apexColor;
	cb_ps_sky.data.centerColor = centerColor;
	this->cb_ps_sky.UpdateBuffer();
	this->cb_ps_screenEffect.UpdateBuffer();

	//waterTranslation += (sin(timer.GetMilisecondsElapsed()/1000)*0.0001f*(float)(rand()%3));

	//OutputDebugStringA(("TIMER = " + std::to_string(tbOffset) + "\n").c_str());


	this->cb_ps_water.UpdateBuffer();
}


void Graphics::DrawObject(GameObject& gameObject, XMMATRIX& projectionMatrix, XMMATRIX& viewMatrix, FrustumClass& frustum, bool culling, bool reconstuct, float xSize, float ySize, float zSize, float distance)
{
	//Draw object with CPU frustum culling

	if (culling)
	{
		XMMATRIX viewProj = viewMatrix * projectionMatrix;
		//if (reconstuct)
		//{
		frustum.ConstructFrustum(distance, viewMatrix, projectionMatrix);
		//}


		XMFLOAT3 tempPos = gameObject.pos;
		//if (gameObject.phsxObj.aActor)
		//{
		//	tempPos = XMFLOAT3(gameObject.phsxObj.trans.p.x, gameObject.phsxObj.trans.p.y, gameObject.phsxObj.trans.p.z);
		//}
	

		bool check = frustum.CheckRect(tempPos.x, tempPos.y, tempPos.z, gameObject.scale.x * gameObject.frustumScale.x, gameObject.scale.y * gameObject.frustumScale.y, gameObject.scale.z * gameObject.frustumScale.z);
		
		gameObject.Draw(viewMatrix, projectionMatrix,true, check);
		
	}
	else
	{
		gameObject.Draw(viewMatrix, projectionMatrix);
	}

}

void Graphics::RenderSceneToTexture(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights,Camera& camera)
{
	sceneTexture.SetRenderTarget(deviceContext.Get(), sceneTexture.m_depthStencilView);
	sceneTexture.ClearRenderTarget(deviceContext.Get(), sceneTexture.m_depthStencilView, rgb[0], rgb[1], rgb[2], rgb[3]);

	PostProccessRender(dt, gameObjects, pointLights,camera);
}


void Graphics::BrdfRender(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader)
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

void Graphics::cubeMapRender(std::vector<GameObject*>& gameObjects, Camera& camera,RenderTexture& currentView, int& index, VertexShader& vertexshader, PixelShader& pixelShader)
{

	currentView.SetRenderTarget(deviceContext.Get(), currentView.m_depthStencilView);

	currentView.ClearRenderTarget(deviceContext.Get(), currentView.m_depthStencilView, rgb[0], rgb[1], rgb[2], rgb[3]);

	XMMATRIX viewMatrix = probeMap.camera[index].GetViewMatrix();
	XMMATRIX projectionMatrix = probeMap.camera[index].GetProjectionMatrix();

	cb_ps_sky.data.apexColor = apexColor;
	cb_ps_sky.data.centerColor = centerColor;
	this->cb_ps_sky.UpdateBuffer();




	this->deviceContext->RSSetState(this->rasterizerState.Get());
	RenderObjects(viewMatrix, projectionMatrix, vertexshader, pixelShader);



	//this->deviceContext->RSSetState(this->rasterizerStateFront.Get());
	//this->deviceContext->VSSetShader(SkyVS.GetShader(), NULL, 0);
	//this->deviceContext->PSSetShader(this->SkyPS.GetShader(), NULL, 0);
	//skydomeObject.SetTranslation(camera.GetPositionFloat3());
	//skydomeObject.SetScale(XMVECTOR{ 4000.0f,4000.0f,4000.0f });
	//skydomeObject.Draw(viewMatrix, projectionMatrix, false);

	this->deviceContext->RSSetState(this->rasterizerState.Get());

	//deviceContext->PSSetShaderResources(4, 1, &blackAndWhiteTexture.shaderResourceView);
	//deviceContext->PSSetShaderResources(5, 1, &cubeMap.shaderResourceView);

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!gameObjects[i]->isTransparent)
		{
			if (!gameObjects[i]->model.deleted)
			{
				if ( !gameObjects[i]->attachToCamera && !gameObjects[i]->model.isAnimated)
				{
					if (gameObjects[i]->model.isAnimated)
					{
						RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, pixelShader);
					}
					else
					{
						RenderObjects(viewMatrix, projectionMatrix, vertexshader, pixelShader);
					}
					if (!gameObjects[i]->isTextured)
					{
						this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
					}
					
					gameObjects[i]->Draw(viewMatrix, projectionMatrix);
				}

			}
		}
	


	}

	this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->isTransparent)
		{
			if (!gameObjects[i]->model.deleted)
			{
				if (!gameObjects[i]->attachToCamera && !gameObjects[i]->model.isAnimated)
				{
					if (gameObjects[i]->model.isAnimated)
					{
						RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, EnvironmentTransparentPS);
					}
					else
					{
						RenderObjects(viewMatrix, projectionMatrix, vertexshader, EnvironmentTransparentPS);
					}
					if (!gameObjects[i]->isTextured)
					{
						this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
					}
					gameObjects[i]->Draw(viewMatrix, projectionMatrix);
				}

			}
		}



	}
	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
	RenderObjects(viewMatrix, projectionMatrix, SkyVS, SkyPS);
	cb_ps_sky.data.apexColor = apexColor;
	cb_ps_sky.data.centerColor = centerColor;
	this->cb_ps_sky.UpdateBuffer();

	this->deviceContext->RSSetState(this->rasterizerStateFront.Get());
	this->deviceContext->VSSetShader(SkyVS.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(SkyPS.GetShader(), NULL, 0);
	skydomeObject.scale = XMFLOAT3(500, 500, 500);
	skydomeObject.pos = camera.GetPositionFloat3();
	skydomeObject.Draw(viewMatrix, projectionMatrix, false);

	this->deviceContext->RSSetState(this->rasterizerState.Get());


}






void Graphics::IrradianceConvolutionRender(Camera& camera, RenderTexture& currentView, int& index, VertexShader& vertexshader, PixelShader& pixelShader)
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


void Graphics::bloomEffect(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader)
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

	//
	//this->deviceContext->PSSetShaderResources(0, 1, &BloomVerticalBlurTexture.shaderResourceView);
	//fullScreenWidow.Draw(camera, depthStencilState2D.Get(), ViewMatrix2D, camera.GetProjectionMatrix(), windowWidth, windowHeight);
}



void Graphics::ForwardRendering(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera)
{
	bool isInEditMode = false;

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->isInEditMode)
		{
			isInEditMode = true;
			break;
		}
	}
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);


	XMMATRIX viewMatrix = camera.GetViewMatrix();
	XMMATRIX projectionMatrix = camera.GetProjectionMatrix();


	deviceContext->PSSetShaderResources(4, 1, &irradianceConvCubeMap.shaderResourceView);
	deviceContext->PSSetShaderResources(5, 1, &brdfTexture.shaderResourceView);
	deviceContext->PSSetShaderResources(6, 1, &prefilterCubeMap[0].shaderResourceView);
	deviceContext->PSSetShaderResources(7, 1, &prefilterCubeMap[1].shaderResourceView);
	deviceContext->PSSetShaderResources(8, 1, &prefilterCubeMap[2].shaderResourceView);
	deviceContext->PSSetShaderResources(9, 1, &prefilterCubeMap[3].shaderResourceView);
	deviceContext->PSSetShaderResources(10, 1, &prefilterCubeMap[4].shaderResourceView);
	deviceContext->PSSetShaderResources(11, 1, &BloomVerticalBlurTexture.shaderResourceView);
	std::vector< ID3D11ShaderResourceView*> ShadowTextures;
	ShadowTextures.resize(pointLights.size());
	int index = 0;
	for (int j = 0; j < pointLights.size(); ++j)
	{

		ShadowTextures[index] = pointLights[j].m_shadowMap.shaderResourceView;
		index++;



	}

	deviceContext->PSSetShaderResources(12, pointLights.size(), ShadowTextures.data());

	

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		

		if (!isInEditMode)
		{
			if (!gameObjects[i]->model.deleted)
			{

				if (!gameObjects[i]->isTransparent)
				{

					this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
					if (gameObjects[i]->model.isAnimated)
					{

						if (gameObjects[i]->isAttachedToObj)
						{
							gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
						}
						RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, pbrPS);


						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, pbrPS);
						}
					}
					else
					{
						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, pbrPS);
						}
						else
						{
							if (gameObjects[i]->isAttachedToObj)
							{
								gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
							}
							RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, pbrPS);
						}
					}



					if (!gameObjects[i]->isTextured)
					{
						this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
					}

					//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
					//{
					//	if (camera.PossessCharacter)
					//	{
					//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//	}
					//
					//}
					//else
					//{
					//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//}
				
					DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
				}

			}
		}
		else
		{
			if (!gameObjects[i]->model.deleted)
			{
				if (gameObjects[i]->isInEditMode)
				{
					if (!gameObjects[i]->isTransparent)
					{

						this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
						if (gameObjects[i]->model.isAnimated)
						{
							if (gameObjects[i]->isAttachedToObj)
							{
								gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
							}

							RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, pbrPS);


							if (gameObjects[i]->attachToCamera)
							{
								RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, pbrPS);
							}
						}
						else
						{
							if (gameObjects[i]->attachToCamera)
							{

								RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, pbrPS);
							}
							else
							{
								if (gameObjects[i]->isAttachedToObj)
								{
									gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
								}

								RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, pbrPS);
							}
						}



						if (!gameObjects[i]->isTextured)
						{
							this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
						}

						//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
						//{
						//	if (camera.PossessCharacter)
						//	{
						//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//	}
						//
						//}
						//else
						//{
						//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//}

						DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					}
				}
				

			}
		}
	}

	if (!isInEditMode)
	{
		RenderObjects(viewMatrix, projectionMatrix, SkyVS, SkyPS);
		cb_ps_sky.data.apexColor = apexColor;
		cb_ps_sky.data.centerColor = centerColor;
		this->cb_ps_sky.UpdateBuffer();
		this->deviceContext->RSSetState(this->rasterizerStateFront.Get());
		this->deviceContext->VSSetShader(SkyVS.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(SkyPS.GetShader(), NULL, 0);
		skydomeObject.scale = XMFLOAT3(500, 500, 500);
		skydomeObject.pos = camera.pos;
		skydomeObject.Draw(viewMatrix, projectionMatrix, false);


		this->deviceContext->RSSetState(this->rasterizerState.Get());

		this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);

		RenderObjects(viewMatrix, projectionMatrix, vertexshader, lightPS);
		//for (int i = 0; i < pointLights.size(); ++i)
		//{
		//	if (lightNames[listbox_light_current] == pointLights[i].lightname)
		//	{
		//	if(pointLights[i].bRenderModel)
		//		pointLights[i].Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix());
		//	}
		//}
	}
	

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		
		if (!gameObjects[i]->isInEditMode)
		{
			if (!gameObjects[i]->model.deleted)
			{

				if (gameObjects[i]->isTransparent)
				{

					this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
					if (gameObjects[i]->model.isAnimated)
					{
						RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, transparentPbrPS);
						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, transparentPbrPS);
						}
					}
					else
					{
						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, transparentPbrPS);
						}
						else
						{
							RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, transparentPbrPS);
						}

					}

					if (!gameObjects[i]->isTextured)
					{
						this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
					}

					//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
					//{
					//	if (camera.PossessCharacter)
					//	{
					//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//	}
					//
					//}
					//else
					//{
					//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//}
			
					DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
				}

			}
		}
		else
		{
			if (gameObjects[i]->isInEditMode)
			{
				if (!gameObjects[i]->model.deleted)
				{

					if (gameObjects[i]->isTransparent)
					{

						this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
						if (gameObjects[i]->model.isAnimated)
						{
							RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, transparentPbrPS);
							if (gameObjects[i]->attachToCamera)
							{
								RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, transparentPbrPS);
							}
						}
						else
						{
							if (gameObjects[i]->attachToCamera)
							{
								RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, transparentPbrPS);
							}
							else
							{
								RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, transparentPbrPS);
							}

						}

						if (!gameObjects[i]->isTextured)
						{
							this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
						}

						//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
						//{
						//	if (camera.PossessCharacter)
						//	{
						//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//	}
						//
						//}
						//else
						//{
						//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//}
						DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					}

				}

			}
			
		}


	}

	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);





	this->deviceContext->OMSetBlendState(AdditiveBlendState.Get(), NULL, 0xFFFFFFFF);

	//if (camera.PossessCharacter)
	//{

	if (!isInEditMode)
	{
		if (muzzleFlash1.particle.attachToCamera)
		{
			RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, ParticlePS);
			this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		else
		{
			RenderObjects(viewMatrix, projectionMatrix, vertexshader, ParticlePS);
			this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		muzzleFlash1.particle.attachToCamera = true;
		muzzleFlash1.particle.scale = XMFLOAT3(0.1, 0.6, 0.6);
		muzzleFlash1.particle.rot = XMFLOAT3(0.0, 1.550, 0.0);
		muzzleFlash1.particle.pos = XMFLOAT3(0.25f, -0.150f, 3.0f);
		muzzleFlash1.particle.frustumEnable = false;
		for (int j = 0; j < gameObjects.size(); ++j)
		{
			if (gameObjects[j]->isFiring || muzzleFlash1.delay < 1.0f)
			{
				muzzleFlash1.delay -= (0.0015 * dt);
				deviceContext->PSSetShaderResources(0, 1, muzzleFlash1.texture.GetTextureResourceViewAddress());
				muzzleFlash1.Draw(viewMatrix, projectionMatrix, true);

			}

			if (muzzleFlash1.delay <= 0.0f)
			{
				muzzleFlash1.delay = 1.0f;
			}
		}


		////////////////
		if (muzzleFlash2.particle.attachToCamera)
		{
			RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, ParticlePS);
			this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		else
		{
			RenderObjects(viewMatrix, projectionMatrix, vertexshader, ParticlePS);
			this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		muzzleFlash2.particle.attachToCamera = true;
		muzzleFlash2.particle.scale = XMFLOAT3(0.1, 0.6, 0.6);
		//muzzleFlash2.particle.rot = XMFLOAT3(0.0, 1.550, 0.0);
		muzzleFlash2.particle.pos = XMFLOAT3(0.25f, -0.150f, 3.0f);
		muzzleFlash2.particle.frustumEnable = false;
		for (int j = 0; j < gameObjects.size(); ++j)
		{
			if (gameObjects[j]->isFiring || muzzleFlash2.delay < 1.0f)
			{
				muzzleFlash2.delay -= (0.0015 * dt);
				deviceContext->PSSetShaderResources(0, 1, muzzleFlash2.texture.GetTextureResourceViewAddress());
				muzzleFlash2.Draw(viewMatrix, projectionMatrix, true);

			}

			if (muzzleFlash2.delay <= 0.0f)
			{
				muzzleFlash2.delay = 1.0f;
			}
		}
	}
	

	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);

}

void Graphics::InitTestScene(Camera& camera)
{
	//timer.Start();
	camera.SetPosition(XMVECTOR{ 0.0f, 3.0f, 0.0f });
	camera.PerspectiveFov(fov, static_cast<float>(windowWidth / windowHeight) * 2, 0.1f, 100000.0f);

}

void Graphics::UpdateBuffers()
{
	this->deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(1, 1, this->cb_ps_sky.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(2, 1, this->cb_ps_water.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(3, 1, this->cb_ps_PCF_Shadows.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(4, 1, this->cb_ps_Deferred_Shading.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(5, 1, this->cb_ps_camera.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(6, 1, this->cb_ps_Pbr.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(8, 1, this->cb_light_global.GetBuffer().GetAddressOf());
	this->deviceContext->PSSetConstantBuffers(9, 1, this->cb_ps_screenEffect.GetBuffer().GetAddressOf());
}


void Graphics::PostProccessRender(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera)
{
	bool isInEditMode = false;

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->isInEditMode)
		{
			isInEditMode = true;
			break;
		}
	}
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	this->deviceContext->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);


	XMMATRIX viewMatrix = camera.GetViewMatrix();
	XMMATRIX projectionMatrix = camera.GetProjectionMatrix();


	deviceContext->PSSetShaderResources(4, 1, &irradianceConvCubeMap.shaderResourceView);
	deviceContext->PSSetShaderResources(5, 1, &brdfTexture.shaderResourceView);
	deviceContext->PSSetShaderResources(6, 1, &prefilterCubeMap[0].shaderResourceView);
	deviceContext->PSSetShaderResources(7, 1, &prefilterCubeMap[1].shaderResourceView);
	deviceContext->PSSetShaderResources(8, 1, &prefilterCubeMap[2].shaderResourceView);
	deviceContext->PSSetShaderResources(9, 1, &prefilterCubeMap[3].shaderResourceView);
	deviceContext->PSSetShaderResources(10, 1, &prefilterCubeMap[4].shaderResourceView);
	deviceContext->PSSetShaderResources(11, 1, &BloomVerticalBlurTexture.shaderResourceView);
	std::vector< ID3D11ShaderResourceView*> ShadowTextures;
	ShadowTextures.resize(pointLights.size());
	int index = 0;
	for (int j = 0; j < pointLights.size(); ++j)
	{

		ShadowTextures[index] = pointLights[j].m_shadowMap.shaderResourceView;
		index++;



	}

	deviceContext->PSSetShaderResources(12, pointLights.size(), ShadowTextures.data());



	for (int i = 0; i < gameObjects.size(); ++i)
	{


		if (!isInEditMode)
		{
			if (!gameObjects[i]->model.deleted)
			{

				if (!gameObjects[i]->isTransparent)
				{

					this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
					if (gameObjects[i]->model.isAnimated)
					{

						if (gameObjects[i]->isAttachedToObj)
						{
							gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
						}
						RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, pbrPS);


						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, pbrPS);
						}
					}
					else
					{
						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, pbrPS);
						}
						else
						{
							if (gameObjects[i]->isAttachedToObj)
							{
								gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
							}
							RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, pbrPS);
						}
					}



					if (!gameObjects[i]->isTextured)
					{
						this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
					}

					//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
					//{
					//	if (camera.PossessCharacter)
					//	{
					//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//	}
					//
					//}
					//else
					//{
					//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//}

					DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
				}

			}
		}
		else
		{
			if (!gameObjects[i]->model.deleted)
			{
				if (gameObjects[i]->isInEditMode)
				{
					if (!gameObjects[i]->isTransparent)
					{

						this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);
						if (gameObjects[i]->model.isAnimated)
						{
							if (gameObjects[i]->isAttachedToObj)
							{
								gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
							}

							RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, pbrPS);


							if (gameObjects[i]->attachToCamera)
							{
								RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, pbrPS);
							}
						}
						else
						{
							if (gameObjects[i]->attachToCamera)
							{

								RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, pbrPS);
							}
							else
							{
								if (gameObjects[i]->isAttachedToObj)
								{
									gameObjects[i]->AttachToObject(*gameObjects[gameObjects[i]->list_box_selected_obj]);
								}

								RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, pbrPS);
							}
						}



						if (!gameObjects[i]->isTextured)
						{
							this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
						}

						//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
						//{
						//	if (camera.PossessCharacter)
						//	{
						//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//	}
						//
						//}
						//else
						//{
						//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//}

						DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					}
				}


			}
		}
	}

	if (!isInEditMode)
	{
		RenderObjects(viewMatrix, projectionMatrix, SkyVS, SkyPS);
		cb_ps_sky.data.apexColor = apexColor;
		cb_ps_sky.data.centerColor = centerColor;
		this->cb_ps_sky.UpdateBuffer();
		this->deviceContext->RSSetState(this->rasterizerStateFront.Get());
		this->deviceContext->VSSetShader(SkyVS.GetShader(), NULL, 0);
		this->deviceContext->PSSetShader(SkyPS.GetShader(), NULL, 0);
		skydomeObject.scale = XMFLOAT3(500, 500, 500);
		skydomeObject.pos = camera.pos;
		skydomeObject.Draw(viewMatrix, projectionMatrix, false);


		this->deviceContext->RSSetState(this->rasterizerState.Get());

		this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);

		RenderObjects(viewMatrix, projectionMatrix, vertexshader, lightPS);
		for (int i = 0; i < pointLights.size(); ++i)
		{
			//if (lightNames[listbox_light_current] == pointLights[i].lightname)
			//{
			//	if (pointLights[i].bRenderModel)
					pointLights[i].Draw(camera.GetViewMatrix(), camera.GetProjectionMatrix());
			//}
		}
	}


	for (int i = 0; i < gameObjects.size(); ++i)
	{

		if (!gameObjects[i]->isInEditMode)
		{
			if (!gameObjects[i]->model.deleted)
			{

				if (gameObjects[i]->isTransparent)
				{

					this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
					if (gameObjects[i]->model.isAnimated)
					{
						RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, transparentPbrPS);
						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, transparentPbrPS);
						}
					}
					else
					{
						if (gameObjects[i]->attachToCamera)
						{
							RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, transparentPbrPS);
						}
						else
						{
							RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, transparentPbrPS);
						}

					}

					if (!gameObjects[i]->isTextured)
					{
						this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
						this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
					}

					//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
					//{
					//	if (camera.PossessCharacter)
					//	{
					//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//	}
					//
					//}
					//else
					//{
					//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					//}

					DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
				}

			}
		}
		else
		{
			if (gameObjects[i]->isInEditMode)
			{
				if (!gameObjects[i]->model.deleted)
				{

					if (gameObjects[i]->isTransparent)
					{

						this->deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);
						if (gameObjects[i]->model.isAnimated)
						{
							RenderObjects(viewMatrix, projectionMatrix, AnimatedVS, transparentPbrPS);
							if (gameObjects[i]->attachToCamera)
							{
								RenderObjects(viewMatrix, projectionMatrix, FPSanimatedVS, transparentPbrPS);
							}
						}
						else
						{
							if (gameObjects[i]->attachToCamera)
							{
								RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, transparentPbrPS);
							}
							else
							{
								RenderObjects(viewMatrix, projectionMatrix, SoftShadowsVS, transparentPbrPS);
							}

						}

						if (!gameObjects[i]->isTextured)
						{
							this->deviceContext->PSSetShaderResources(0, 1, noTextureAlbedo.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(1, 1, noTextureNormal.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(2, 1, noTextureRoughness.GetTextureResourceViewAddress());
							this->deviceContext->PSSetShaderResources(3, 1, noTextureMetallic.GetTextureResourceViewAddress());
						}

						//if (gameObjects[i]->isPlayer || gameObjects[i]->attachToCamera)
						//{
						//	if (camera.PossessCharacter)
						//	{
						//		DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//	}
						//
						//}
						//else
						//{
						//	DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
						//}
						DrawObject(*gameObjects[i], projectionMatrix, viewMatrix, frustums[i], gameObjects[i]->frustumEnable, true, 1, 1, 1, 3000);
					}

				}

			}

		}


	}

	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);





	this->deviceContext->OMSetBlendState(AdditiveBlendState.Get(), NULL, 0xFFFFFFFF);

	//if (camera.PossessCharacter)
	//{

	if (!isInEditMode)
	{
		if (muzzleFlash1.particle.attachToCamera)
		{
			RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, ParticlePS);
			this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		else
		{
			RenderObjects(viewMatrix, projectionMatrix, vertexshader, ParticlePS);
			this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		muzzleFlash1.particle.attachToCamera = true;
		muzzleFlash1.particle.scale = XMFLOAT3(0.1, 0.6, 0.6);
		muzzleFlash1.particle.rot = XMFLOAT3(0.0, 1.550, 0.0);
		muzzleFlash1.particle.pos = XMFLOAT3(0.25f, -0.150f, 3.0f);
		muzzleFlash1.particle.frustumEnable = false;
		for (int j = 0; j < gameObjects.size(); ++j)
		{
			if (gameObjects[j]->isFiring || muzzleFlash1.delay < 1.0f)
			{
				muzzleFlash1.delay -= (0.0015 * dt);
				deviceContext->PSSetShaderResources(0, 1, muzzleFlash1.texture.GetTextureResourceViewAddress());
				muzzleFlash1.Draw(viewMatrix, projectionMatrix, true);

			}

			if (muzzleFlash1.delay <= 0.0f)
			{
				muzzleFlash1.delay = 1.0f;
			}
		}


		////////////////
		if (muzzleFlash2.particle.attachToCamera)
		{
			RenderObjects(viewMatrix, projectionMatrix, FPScameraVS, ParticlePS);
			this->deviceContext->VSSetShader(this->FPScameraVS.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		else
		{
			RenderObjects(viewMatrix, projectionMatrix, vertexshader, ParticlePS);
			this->deviceContext->VSSetShader(this->vertexshader.GetShader(), NULL, 0);
			this->deviceContext->PSSetShader(this->ParticlePS.GetShader(), NULL, 0);
		}
		muzzleFlash2.particle.attachToCamera = true;
		muzzleFlash2.particle.scale = XMFLOAT3(0.1, 0.6, 0.6);
		//muzzleFlash2.particle.rot = XMFLOAT3(0.0, 1.550, 0.0);
		muzzleFlash2.particle.pos = XMFLOAT3(0.25f, -0.150f, 3.0f);
		muzzleFlash2.particle.frustumEnable = false;
		for (int j = 0; j < gameObjects.size(); ++j)
		{
			if (gameObjects[j]->isFiring || muzzleFlash2.delay < 1.0f)
			{
				muzzleFlash2.delay -= (0.0015 * dt);
				deviceContext->PSSetShaderResources(0, 1, muzzleFlash2.texture.GetTextureResourceViewAddress());
				muzzleFlash2.Draw(viewMatrix, projectionMatrix, true);

			}

			if (muzzleFlash2.delay <= 0.0f)
			{
				muzzleFlash2.delay = 1.0f;
			}
		}
	}


	this->deviceContext->OMSetBlendState(NoBlendState.Get(), NULL, 0xFFFFFFFF);

}