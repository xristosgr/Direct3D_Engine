#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndicesBuffer.h"
#include "ConstantBufferTypes.h"
#include "ConstantBuffer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"
#include "VertexBoneData.h"
//#include "TextureVariables.h"
//#include<Windows.h>

struct BoneMatrix
{
	DirectX::XMMATRIX BoneOffset;
	DirectX::XMMATRIX FinalTransformation;
};

class Mesh 
{
public:
	Mesh();
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, std::vector<Texture>& textures, DirectX::XMMATRIX transformMatrix);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices,std::vector<Texture>& textures,std::vector<VertexBoneData>& bones,DirectX::XMMATRIX transformMatrix);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, DirectX::XMMATRIX transformMatrix);
	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, DirectX::XMMATRIX transformMatrix);

	Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices,std::vector<VertexBoneData>& bones, DirectX::XMMATRIX transformMatrix);
	
	void Draw(bool isTextured, bool isPBR, bool isInstanced);
	void DrawUntextured();
	void DrawCube(bool isTextured, bool isPBR);
	void DrawRect(bool isTextured, bool isPBR);
	void DrawShape(UINT& vertexCount, bool isTextured, bool isPBR);
	const DirectX::XMMATRIX& GetTranformMatrix();
	void SetTexture(std::vector<Texture>& textures);
	void UpdateVertices(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::vector<Vertex>& vertices);

	VertexBuffer<VertexBoneData> boneDataBuffer;
	VertexBuffer<Vertex> vertexBuffer;
	std::vector<Texture> textures;

	bool deleted = false;
private:
	

	
	IndicesBuffer indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	DirectX::XMMATRIX transformMatrix;
};
