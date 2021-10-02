#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Color.h"
#include <string>
#include <assimp/material.h>
#include <DirectXMath.h>
#include "stb_image.h"

enum class TextureStorageType
{
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};

class Texture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};
	int id;
public:

	Texture() {};
	~Texture() {};
	Texture(ID3D11Device* device, const Color& color, aiTextureType type);
	Texture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type); //Generate texture of specific color data
	Texture(ID3D11Device* device, const std::string& filePath, aiTextureType type);
	Texture(ID3D11Device* device, const uint8_t* pData, size_t size, aiTextureType type);
	Texture(ID3D11Device* device, const std::string& filePath);
	aiTextureType GetType();
	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();
	void CreateTextureTGA(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath);
	void CreateTextureDDS(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath);
	void CreateTextureWIC(ID3D11Device* device, const std::string& filePath);
	void LoadHdrTexture(ID3D11Device* device, const char* filePath);
	void LoadStbiTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filePath);
	void LoadCubeTexture(ID3D11Device* device, const char** filePaths);
	//bool operator<(const Texture& src)const
	//{
	//	return (this->id < src.id);
	//}
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;

	//int width = 0;
	//int height = 0;
	//unsigned char* data = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	D3D11_TEXTURE2D_DESC desc;
	std::string path;

	Microsoft::WRL::ComPtr < ID3D11Texture2D> mTexture;
private:
	void Initialize1x1ColorTexture(ID3D11Device* device, const Color& colorData, aiTextureType type);
	void InitializeColorTexture(ID3D11Device* device, const Color* colorData, UINT width, UINT height, aiTextureType type);
	bool LoadTGAFile(const std::string& filePath, int& height, int& width);
	
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;

	unsigned char* m_targaData = 0;

};