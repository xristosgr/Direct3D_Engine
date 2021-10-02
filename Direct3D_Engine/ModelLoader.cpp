#include "ModelLoader.h"
#include <thread>



bool ModelLoader::Initialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool isAnimated, bool isPBR,bool isTextured)
{
	currAnim = 0;
	//CoInitialize(NULL);
	_filepath = std::string(filePath);
	this->device = device;
	this->deviceContext = deviceContex;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	//this->blenderModel = blenderModel;
	this->isAnimated = isAnimated;
	this->isPBR = isPBR;
	this->isTextured = isTextured;


	_curScene = NULL;
	_prevAnimIndex = -1;
	_blendingTime = 0.0f;
	_blendingTimeMul = 1.0f;
	_updateBoth = true;
	_temporary = false;
	_playTime = 0.0f;


	if (std::string::npos != filePath.find(".gltf"))
	{
		const size_t lastShlashPos = filePath.rfind('/');
		if (std::string::npos >= lastShlashPos)
		{
		
			fileDirectory = filePath.substr(0, lastShlashPos);
		}


		this->GLTF = true; 
	}
	else if(std::string::npos != filePath.find(".glb"))
	{
		const size_t lastShlashPos = filePath.rfind('/');
		if (std::string::npos >= lastShlashPos)
		{
			fileDirectory = filePath.substr(0, lastShlashPos);
		}

		this->GLTF = true;
	}
	else if (std::string::npos != filePath.find(".dae"))
	{
		const size_t lastShlashPos = filePath.rfind('/');
		if (std::string::npos >= lastShlashPos)
		{
			
			fileDirectory = filePath.substr(0, lastShlashPos);
		}
		this->GLTF = false;
	}
	else if (std::string::npos != filePath.find(".obj"))
	{
		const size_t lastShlashPos = filePath.rfind('/');
		if (std::string::npos >= lastShlashPos)
		{
			fileDirectory = filePath.substr(0, lastShlashPos);
		}
		this->GLTF = false;
	}
	else if (std::string::npos != filePath.find(".fbx"))
	{
		const size_t lastShlashPos = filePath.rfind('/');
		if (std::string::npos >= lastShlashPos)
		{
			fileDirectory = filePath.substr(0, lastShlashPos);
		}
		this->GLTF = false;
	}

	//if (GLTF)
	//{
	//	animSpeed = 1000.0f;
	//}

	
	


	//try
	//{
		//if (!deleted)
		//{
		//	if (!this->LoadModel(&filePath, &isTextured))
		//	{
		//		//return false;
		//	}
		//}
		
	//}

	//catch (COMException & exception)
	//{
	//	ErrorLogger::Log(exception);
	//	return false;
	//}
	return true;
}
void ModelLoader::SetTextures(bool& isPBR, Texture tex[4])
{
	_textures.push_back(tex[0]);
	if (isPBR)
	{
		_textures.push_back(tex[1]);
		_textures.push_back(tex[2]);
		_textures.push_back(tex[3]);
	}
	texuresLoaded = true;
}

void ModelLoader::SetTextures(bool& isPBR, const std::vector<Texture>& tex)
{
	_textures.push_back(tex[0]);
	if (isPBR)
	{
		_textures.push_back(tex[1]);
		_textures.push_back(tex[2]);
		_textures.push_back(tex[3]);
	}
	texuresLoaded = true;
}


void ModelLoader::SetTextures(bool& isPBR)
{

	std::stringstream ssAlbedo;
	ssAlbedo << "/albedo.dds";
	std::string albedoTexPath = fileDirectory + ssAlbedo.str();
	//OutputDebugStringA((fileDirectory + "\n").c_str());

	std::stringstream ssNormal;
	ssNormal << "/normal.dds";
	std::string normalTexPath = fileDirectory + ssNormal.str();
	//OutputDebugStringA((normalTexPath + "\n").c_str());

	std::stringstream ssRoughness;
	ssRoughness << "/roughness.dds";
	std::string roughnessTexPath = fileDirectory + ssRoughness.str();
	//OutputDebugStringA((roughnessTexPath + "\n").c_str());

	std::stringstream ssMetallic;
	ssMetallic << "/metallic.dds";
	std::string metallicTexPath = fileDirectory + ssMetallic.str();
	//OutputDebugStringA((metallicTexPath + "\n").c_str());



	Texture albedo, normal, roughness, metallic;

	albedo.CreateTextureDDS(device.Get(), deviceContext.Get(), albedoTexPath);

	if (isPBR)
	{
	
		normal.CreateTextureDDS(device.Get(), deviceContext.Get(), normalTexPath);
		roughness.CreateTextureDDS(device.Get(), deviceContext.Get(), roughnessTexPath);
		metallic.CreateTextureDDS(device.Get(), deviceContext.Get(), metallicTexPath);
		
	
	}

	_textures.push_back(albedo);
	if (isPBR)
	{
		_textures.push_back(normal);
		_textures.push_back(roughness);
		_textures.push_back(metallic);
	}
	
	texuresLoaded = true;
}


void ModelLoader::Draw(XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured, XMMATRIX& parentWorldMatrix, XMMATRIX& parentBoneWorldMatrix)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());


	if (isAnimated)
	{
		if (bonesLoaded)
		{
			XMFLOAT4X4 _transforms;
			std::vector<XMMATRIX> transforms;
			BoneTransform(transforms);
			XMStoreFloat4x4(&_transforms, transforms[0]);

			//calculateAnims = std::async(std::launch::async, &Animation::BoneTransform, this, std::ref(transforms));



			for (unsigned int i = 0; i < transforms.size(); ++i)
			{
				this->cb_vs_vertexshader->data.bones_transform[i] = transforms[i];


			}
			this->cb_vs_vertexshader->data.viewMatrix = XMMatrixTranspose(viewMatrix);
			this->cb_vs_vertexshader->data.projectionMatrix = XMMatrixTranspose(projectionMatrix);
		}

	}
	else
	{
		this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
		this->cb_vs_vertexshader->data.projectionMatrix = projectionMatrix;
	}


	if (!isAnimated)
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
		
			this->cb_vs_vertexshader->data.worldMatrix =   XMMatrixTranspose( meshes[i].GetTranformMatrix())*  worldMatrix * parentWorldMatrix;
		
			//this->cb_vs_vertexshader->data.wvpMatrix = parentBoneWorldMatrix * XMMatrixTranspose(meshes[i].GetTranformMatrix())* parentBoneWorldMatrix * parentWorldMatrix*worldMatrix * viewMatrix * projectionMatrix;
			
		
		
			this->cb_vs_vertexshader->UpdateBuffer();
		
			meshes[i].Draw(isTextured, isPBR,isInstancedObject);
		}
		//this->cb_vs_vertexshader->data.worldMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix()) * worldMatrix * parentWorldMatrix;
		//
		//this->cb_vs_vertexshader->data.wvpMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix()) * worldMatrix * parentWorldMatrix * viewMatrix * projectionMatrix;
		//
		//
		//
		//this->cb_vs_vertexshader->UpdateBuffer();
		//
		//meshes[0].Draw(isTextured, isPBR);
	}
	else
	{

		if (meshes.size() > 0)
		{
			this->cb_vs_vertexshader->data.worldMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix() * worldMatrix * parentWorldMatrix);
			this->cb_vs_vertexshader->data.wvpMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix() * worldMatrix * parentWorldMatrix *  viewMatrix * projectionMatrix);
			this->cb_vs_vertexshader->UpdateBuffer();
			meshes[0].Draw(this->isTextured, isPBR, isInstancedObject);


		}

	}

}
void ModelLoader::Draw(XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured,bool isPlayer)
{
	if (!cb_vs_vertexshader)
		return;
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());


	if (isAnimated)
	{
		if (bonesLoaded)
		{
			XMFLOAT4X4 _transforms;
			std::vector<XMMATRIX> transforms;
			BoneTransform(transforms);
			XMStoreFloat4x4(&_transforms, transforms[0]);
			
			//calculateAnims = std::async(std::launch::async, &Animation::BoneTransform, this, std::ref(transforms));



			for (unsigned int i = 0; i < transforms.size(); ++i)
			{
				this->cb_vs_vertexshader->data.bones_transform[i] = transforms[i];


			}
			this->cb_vs_vertexshader->data.viewMatrix = XMMatrixTranspose(viewMatrix);
			this->cb_vs_vertexshader->data.projectionMatrix = XMMatrixTranspose(projectionMatrix);
		}

	}
	else
	{
		this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
		this->cb_vs_vertexshader->data.projectionMatrix = projectionMatrix;
	}



	
	if (!isAnimated)
	{
		//OutputDebugStringA(_filepath.c_str());
		//OutputDebugStringA(("\nSIZE  = " + std::to_string(meshes.size())+"\n").c_str());
		for (int i = 0; i < meshes.size(); ++i)
		{
		
			this->cb_vs_vertexshader->data.worldMatrix = XMMatrixTranspose(meshes[i].GetTranformMatrix()) * worldMatrix;
			this->cb_vs_vertexshader->data.wvpMatrix = XMMatrixTranspose(meshes[i].GetTranformMatrix()) * worldMatrix * viewMatrix * projectionMatrix;
			this->cb_vs_vertexshader->UpdateBuffer();
			if(!isPlayer)
				meshes[i].Draw(isTextured, isPBR, isInstancedObject);
		}

		//this->cb_vs_vertexshader->data.worldMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix()) * worldMatrix;
		//this->cb_vs_vertexshader->data.wvpMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix()) * worldMatrix * viewMatrix * projectionMatrix;
		//this->cb_vs_vertexshader->UpdateBuffer();
		//if (!isPlayer)
		//	meshes[0].Draw(isTextured, isPBR);
		

	}
	else
	{

		if (meshes.size() > 0)
		{

			this->cb_vs_vertexshader->data.worldMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix() * worldMatrix);
			this->cb_vs_vertexshader->data.wvpMatrix = XMMatrixTranspose(meshes[0].GetTranformMatrix() * worldMatrix * viewMatrix * projectionMatrix);
			this->cb_vs_vertexshader->UpdateBuffer();
			if(!isPlayer)
				meshes[0].Draw(this->isTextured, isPBR, isInstancedObject);


		}
	
	}

	this->worldMatrix = worldMatrix;
}

void ModelLoader::DrawBones(XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured)
{

	//this->cb_vs_vertexshader->data.worldMatrix = _M;
	//this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
	//this->cb_vs_vertexshader->data.projectionMatrix = projectionMatrix;
	//cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;
	//this->cb_vs_vertexshader->UpdateBuffer();
	//
 	//
	//if(boneVertices.size()>0)
	//	skeletonMeshes.push_back(Mesh(device, deviceContext, boneVertices, worldMatrix));
	//
	//if(skeletonMeshes.size()>0)
	//	skeletonMeshes[0].Draw(false, false);
	//
	//this->boneVertices.clear();
	//skeletonMeshes.clear();
}

bool ModelLoader::LoadModel(const std::string filePath, bool isTextured)
{
	
	
	this->directory = StringHelper::GetDirectoryFromPath(filePath);

	_curScene = importers.size();
	importers.push_back(new Assimp::Importer);

	scenes.push_back(importers[_curScene]->ReadFile(filePath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_SplitLargeMeshes | aiProcess_OptimizeMeshes | aiProcess_FixInfacingNormals | aiProcess_ImproveCacheLocality | aiProcess_FindInvalidData | aiProcess_RemoveRedundantMaterials));
	//scenes.push_back(importers[_curScene]->ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights | aiProcess_ValidateDataStructure | aiProcess_FlipWindingOrder | aiProcess_FixInfacingNormals | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_FindInstances));

	if (!scenes[_curScene] || scenes[_curScene]->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scenes[_curScene]->mRootNode)
	{
		ErrorLogger::Log(importers[_curScene]->GetErrorString());
		return false;
	}
	
	aiMatrix4x4 mTransform = scenes[_curScene]->mRootNode->mTransformation;
	mTransform.Inverse();

	m_GlobalInverseTransform = XMMatrixTranspose(XMMATRIX(&mTransform.a1));
	this->ProcessNode(scenes[_curScene]->mRootNode, scenes[_curScene], DirectX::XMMatrixIdentity(), isTextured);


	//for (int i = 0; i < scenes.size(); ++i)
	//{
	//	//for (int j = 0; j < scenes[i]->mNumTextures; ++j)
	//	//{
	//	//	scenes[i]->mTextures[j]->~aiTexture();
	//	//}
	//	for (int j = 0; j < scenes[i]->mNumCameras; ++j)
	//	{
	//		scenes[i]->mCameras[j]->~aiCamera();
	//	}
	//
	//	//for (int j = 0; j < scenes[i]->mNumMaterials; ++j)
	//	//{
	//	//	scenes[i]->mMaterials[j]->~aiMaterial();
	//	//}
	//}

	return true;
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransformMatrix, bool& TexturesLoaded)
{

	XMMATRIX nodeTransformMatrix = XMMATRIX(&node->mTransformation.a1) * parentTransformMatrix;
	std::vector<Mesh> tempMesh;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		

		meshes.push_back(this->ProcessMesh(mesh, scene, nodeTransformMatrix, TexturesLoaded));

		
	}

	
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTransformMatrix, TexturesLoaded);
	}


}

Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& transformMatrix,bool& isTextured)
{

	// Data to fill

	std::vector<VertexBoneData> bones;


	bones.resize(mesh->mNumVertices);

	for (UINT i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (mesh->mNormals != NULL)
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}
		else
		{
			vertex.normal.x = mesh->mVertices[i].x;
			vertex.normal.y = mesh->mVertices[i].y;
			vertex.normal.z = mesh->mVertices[i].z;
		}


		if (mesh->mTangents != NULL)
		{
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;
		}
		else
		{
			// Generate tangents
			DirectX::XMVECTOR normalVec = XMLoadFloat3(&vertex.normal);

			DirectX::XMFLOAT3 tempFloat3 = DirectX::XMFLOAT3(0.0f, 0.0f, 0.1f);
			DirectX::XMVECTOR tempVec = XMLoadFloat3(&tempFloat3);
			DirectX::XMVECTOR t1 = DirectX::XMVector3Cross(normalVec, tempVec);

			tempFloat3 = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			tempVec = XMLoadFloat3(&tempFloat3);
			DirectX::XMVECTOR t2 = DirectX::XMVector3Cross(normalVec, tempVec);

			DirectX::XMVECTOR t1LengthVec = DirectX::XMVector3Length(t1);
			DirectX::XMFLOAT3 t1LengthFloat3;
			XMStoreFloat3(&t1LengthFloat3, t1LengthVec);

			DirectX::XMVECTOR t2LengthVec = DirectX::XMVector3Length(t2);
			DirectX::XMFLOAT3 t2LengthFloat3;
			XMStoreFloat3(&t2LengthFloat3, t2LengthVec);

			if (t1LengthFloat3.x > t2LengthFloat3.x&& t1LengthFloat3.y > t2LengthFloat3.y&& t1LengthFloat3.z > t2LengthFloat3.z)
			{
				XMStoreFloat3(&vertex.tangent, t1);
			}
			else
			{
				XMStoreFloat3(&vertex.tangent, t2);
			}
		}

		if (mesh->mBitangents != NULL)
		{
			vertex.binormal.x = mesh->mBitangents[i].x;
			vertex.binormal.y = mesh->mBitangents[i].y;
			vertex.binormal.z = mesh->mBitangents[i].z;
		}
		else
		{
			// generate binormals(bitangents)
			DirectX::XMVECTOR tangent = XMLoadFloat3(&vertex.tangent);
			DirectX::XMVector3Normalize(tangent);
			DirectX::XMVECTOR binormal = XMLoadFloat3(&vertex.binormal);
			DirectX::XMVECTOR normal = XMLoadFloat3(&vertex.normal);

			binormal = DirectX::XMVector3Cross(tangent, normal);

			XMStoreFloat3(&vertex.binormal, binormal);
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);

	}

		
	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	//std::vector<Texture> textures;
	std::string textPath;
	if (!deleted)
	{
		
		if (this->isTextured)
		{
			//SetTextures(isPBR);

			if (isAnimated)
			{
				bonesLoaded = LoadBones(mesh, bones, vertices);

				if(bonesLoaded)
					return Mesh(this->device, this->deviceContext, vertices, indices, _textures, bones, transformMatrix);
			}


			return Mesh(this->device, this->deviceContext, vertices, indices, _textures, transformMatrix);
		}
		else
		{
			if (isAnimated)
			{
				bonesLoaded = LoadBones(mesh, bones, vertices);
				
				if(bonesLoaded)
					return Mesh(this->device, this->deviceContext, vertices, indices, bones, transformMatrix);
			}
		
		
			return Mesh(this->device, this->deviceContext, vertices, indices, transformMatrix);
		}
	}
		

}
