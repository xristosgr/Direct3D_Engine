#pragma once

#include "Animation.h"
#include <map>
#include <sstream>
#include "Instance.h"

class ModelLoader : public Animation
{
public:

	bool Initialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool isAnimated, bool isPBR, bool isTextured);
	void SetTextures(bool& isPBR, Texture tex[4]);
	void SetTextures(bool& isPBR, const std::vector<Texture>& tex);
	//bool Initialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool& isAnimated, bool& isPBR);

	void SetTextures(bool& isPBR);

	bool isAnimated;
	std::vector<Vertex> vertices;
	std::vector<UINT32> indices;
	std::vector<Instance> instances;


	std::vector<Mesh> meshes;
	std::vector<Mesh> skeletonMeshes;

	std::map<int, std::vector<Mesh>> AnimMeshes;
	std::vector<Texture> _textures;


	bool isInstancedObject = false;
	bool deleted = false;
	bool texuresLoaded = false;
private:


	bool texturesHasBeenLoaded = false;

	bool GLTF = false;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader = nullptr;

	std::string directory = "";

	//bool blenderModel;

	bool isPBR;
	bool isTextured;




	// bones data
	//aiMatrix4x4 m_GlobalInverseTransform;

public:
	void Draw(XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured, XMMATRIX& parentWorldMatrix, XMMATRIX& parentBoneWorldMatrix);
	void Draw(XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured, bool isPlayer = false);
	void DrawBones(XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured);

	bool LoadModel(const std::string filePath, bool isTextured);

private:
	void ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix, bool& TexturesLoaded);

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix, bool& TexturesLoaded);


	const std::string fp;
	char* f;


	std::string fileDirectory;


public:
	std::string* textures;
	std::string _filepath;

	XMMATRIX worldMatrix;

};
