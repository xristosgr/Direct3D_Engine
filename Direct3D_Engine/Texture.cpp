#include "Texture.h"
#include "ErrorLogger.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>


Texture::Texture(ID3D11Device* device, const Color& color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type)
{
	this->InitializeColorTexture(device, colorData, width, height, type);
}

Texture::Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type)
{
	this->type = type;
	if (StringHelper::GetFileExtension(filePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		path = filePath;

		return;
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		path = filePath;

		return;
	}
}

Texture::Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type)
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, this->texture.GetAddressOf(), this->textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
}

Texture::Texture(ID3D11Device* device, const std::string& filePath)
{
	//if (StringHelper::GetFileExtension(filePath) == ".dds")
	//{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
		path = filePath;
	//}
	//HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
	//if (FAILED(hr))
	//{
	//	this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
	//}
	//return;
}

aiTextureType Texture::GetType()
{
	return this->type;
}

ID3D11ShaderResourceView* Texture::GetTextureResourceView()
{
	return this->textureView.Get();
}

ID3D11ShaderResourceView** Texture::GetTextureResourceViewAddress()
{
	return this->textureView.GetAddressOf();
}

void Texture::CreateTextureTGA(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	result = LoadTGAFile(filePath, width, height);

	if (!result)
	{
		ErrorLogger::Log(filePath);
		return;
	}

	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	hResult = device->CreateTexture2D(&textureDesc, NULL, m_texture.GetAddressOf());
	if (FAILED(hResult))
	{
		COM_ERROR_IF_FAILED(hResult, "Failed to create 2D texture.");
	}

	rowPitch = (width * 4) * sizeof(unsigned char);

	deviceContext->UpdateSubresource(m_texture.Get(), 0, NULL, m_targaData, rowPitch, 0);

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hResult = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, textureView.GetAddressOf());
	if (FAILED(hResult))
	{
		COM_ERROR_IF_FAILED(hResult, "Failed to create shader resourceView.");
	}
	deviceContext->GenerateMips(textureView.Get());

	delete[] m_targaData;
	m_targaData = 0;
}

void Texture::CreateTextureDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath)
{

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _tempView = nullptr;
	//HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());


	HRESULT hr;

	hr = DirectX::CreateDDSTextureFromFileEx(device,
		deviceContext,
		StringHelper::StringToWide(filePath).c_str(),
		0Ui64,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		0,
		D3D11_RESOURCE_MISC_GENERATE_MIPS,
		false,
		this->texture.GetAddressOf(),
		textureView.GetAddressOf()
	);
	COM_ERROR_IF_FAILED(hr, "Failed to create Texture from file: "+ filePath);
	path = filePath;
}

void Texture::CreateTextureWIC(ID3D11Device* device, const std::string& filePath)
{
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(filePath).c_str(), texture.GetAddressOf(), this->textureView.GetAddressOf());
	if (FAILED(hr))
	{
		COM_ERROR_IF_FAILED(hr, "Failed to create Texture from memory.");
		this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
	}
	path = filePath;
}

void Texture::LoadHdrTexture(ID3D11Device* device, const char* filePath)
{
	try
	{
		int width = 0;
		int height = 0;
		int nrComponents;
		unsigned char* data = stbi_load(filePath, &width, &height, NULL, 4);

		if (data)
		{
			D3D11_TEXTURE2D_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Width = width;
			desc.Height = height;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
		

			ID3D11Texture2D* pTexture = NULL;
			D3D11_SUBRESOURCE_DATA subResource;
			subResource.pSysMem = data;
			subResource.SysMemPitch = desc.Width  * 4;
			subResource.SysMemSlicePitch = 0;
			HRESULT hr = device->CreateTexture2D(&desc, &subResource, &pTexture);
			COM_ERROR_IF_FAILED(hr, "Failed to create texture");
			// Create texture view
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			hr = device->CreateShaderResourceView(pTexture, &srvDesc, &textureView);
			COM_ERROR_IF_FAILED(hr, "Failed to CreateShaderResourceView");
		}
	}
	catch (COMException exc)
	{
		ErrorLogger::Log(exc);
		return;
	}

}

void Texture::LoadStbiTexture(ID3D11Device* device,ID3D11DeviceContext* deviceContext, const char* filePath)
{
	try
	{
		int width = 0;
		int height = 0;
		int nrComponents;
		
		unsigned char* data = stbi_load(filePath, &width, &height, &nrComponents, 0);
		if (!data)
		{
			ErrorLogger::Log("Failed to load data.");
		}
		
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 0;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = data;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		HRESULT hr = device->CreateTexture2D(&desc, &subResource, m_texture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create texture");
		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		hr = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, &textureView);
		COM_ERROR_IF_FAILED(hr, "Failed to CreateShaderResourceView");

	
		deviceContext->GenerateMips(textureView.Get());




		
	}
	catch (COMException exc)
	{
		ErrorLogger::Log(exc);
		return;
	}

}
void Texture::LoadCubeTexture(ID3D11Device* device, const char** filePaths)
{


	int width = 0;
	int height = 0;
	int nrComponents;
	unsigned char* data[6];
	for (int i = 0; i < 6; i++)
	{
		data[i] = stbi_load(filePaths[i], &width, &height, 0, 4);



	}
	
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	
	desc.Width = width;
	desc.Height = height;
	
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	//desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource[6];
	for (int i = 0; i < 6; i++)
	{
	
		subResource[i].pSysMem = data[i];
	
		subResource[i].SysMemPitch = desc.Width * 4;
		subResource[i].SysMemSlicePitch = 0;
	}
	
	HRESULT hr = device->CreateTexture2D(&desc, subResource, &pTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to create texture");
	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(pTexture, &srvDesc, &textureView);
	COM_ERROR_IF_FAILED(hr, "Failed to CreateShaderResourceView");

}
void Texture::Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData, aiTextureType type)
{
	InitializeColorTexture(device, &colorData, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type)
{
	this->type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	ID3D11Texture2D* p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = width * sizeof(Color);
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize texture from color data.");
	texture = static_cast<ID3D11Texture2D*>(p2DTexture);
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create shader resource view from texture generated from color data.");
}

bool Texture::LoadTGAFile(const std::string& filename, int& height, int& width)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	error = fopen_s(&filePtr, filename.data(), "rb");
	if (error != 0)
	{
		//ErrorLogger::Log("line 393.");
		return false;
	}

	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		//ErrorLogger::Log("line 400.");
		return false;
	}
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;
	if (bpp != 32)
	{
		//ErrorLogger::Log("line 408.");
		return false;
	}

	imageSize = width * height * 4;

	
	targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		//ErrorLogger::Log("line 416.");
		return false;
	}
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		//ErrorLogger::Log("line 422.");
		return false;
	}
	error = fclose(filePtr);
	if (error != 0)
	{
		//ErrorLogger::Log("line 428.");
		return false;
	}
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		//ErrorLogger::Log("line 434.");
		return false;
	}

	index = 0;

	k = (width * height * 4) - (width * 4);

	for (j = 0; j < height; ++j)
	{
		for (i = 0; i < width; ++i)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

	
						// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}
		k -= (width * 8);
	}
	


	delete[] targaImage;
	targaImage = 0;

	return true;
}
