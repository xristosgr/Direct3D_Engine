#pragma once


#include"ShadowRenderer.h"
#include<string>

#include <fstream>
#include "SaveSystem.h"
#include "InputVariables.h"

#include"ParticleObject.h"
#include "PhysXDebugDraw.h"
#include "PhysXHandler.h"
#include "ModelViewer.h"
#include "UIClass.h"
#include "NavMeshClass.h"
#include "PostProccessing.h"

class Graphics : public ShadowRenderer, public SaveSystem, public PostProccessing
{
public:

	bool Initialize(HWND hwnd, Camera& camera, int width, int height);
	void RenderFrame(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera, PhysXHandler& physXHandler, PhysXDebugDraw& physXDebugDraw, bool& HideUI, int& listbox_item_current, std::vector< const char*>& objNames, KeyboardClass& keyboard, MouseClass& mouse, float& mouseX, float& mouseY, int screenWidth, int screenHeight, bool& rightDown, int& selectedObject, GridClass& grid, NavMeshClass& navMesh);
	void SetDebugWindowMode(int debugWindowMode);
	void InitializeObjects(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera, std::vector< const char*>& objNames);
	void PostProccessRender(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera);
	int vSync = 0;

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene(Camera& camera);
	bool initPSShader(PixelShader* pixelShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath);
	bool initVSShader(VertexShader* vertexShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT* numElements);
	bool initComputeShader(ComputeShader* computeShader, Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring filePath);

	void CreateLight(std::vector<PointLight>& pointLights, ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void CreateShadowMap();

	void FPSCounter();
	void RenderGui(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera, KeyboardClass& keyboard, int& listbox_item_current, std::vector< const char*>& objNames, GridClass& grid);
	bool CreateObject(std::vector<GameObject*>& gameObjects,std::string fileName, bool isAnimated, bool isPBR, bool isTextured, bool isEmptyTarget, bool isObjectDeleted);
	void AddObject(std::vector<GameObject*>& gameObjects, int& listbox_item_current, std::vector< const char*>& objNames);
	void AddLight(std::vector<PointLight>& pointLights);

	

private:
	void DrawObject(GameObject& gameObject, XMMATRIX& projectionMatrix, XMMATRIX& viewMatrix, FrustumClass& frustum, bool culling = true, bool reconstuct = true, float xSize = 1, float ySize = 1, float zSize = 1, float distance = 1000);
	void RenderObjects(XMMATRIX&, XMMATRIX&, VertexShader& vertexshader, PixelShader& pixelShader);
	void UpdateShaders(std::vector<PointLight>& pointLights, Camera& camera);

	void RenderSceneToTexture(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera);
	//void BrdfRender(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader);

	void cubeMapRender(std::vector<GameObject*>& gameObjects, Camera& camera, RenderTexture& currentView, int& index, VertexShader& vertexshader, PixelShader& pixelShader);
	//void IrradianceConvolutionRender(Camera& camera, RenderTexture& currentView, int& index, VertexShader& vertexshader, PixelShader& pixelShader);
	//void bloomEffect(Camera& camera, RenderTexture& texture, VertexShader& vertexshader, PixelShader& pixelShader);

	void ForwardRendering(float& dt, std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, Camera& camera);
	void InitTestScene(Camera& camera);

	void UpdateBuffers();

	std::string inName;
	//char inName[100] = "Data/Objects/pbrTest/";
	bool addObject = false;
	bool vSyncEnabled = false;
	

	

	std::fstream f;


	bool isAnim = false;
	bool isPbr = false;
	bool isTex = false;

	int selectedObject = 0;

	ModelViewer modelViewer;
private:
	std::vector<ID3D11Texture2D*> pTexts;
	bool TextureInit = false;
	bool TextureInit2 = false;
	bool addLight = false;

	bool emptyTarget = false;

	
	std::future<void> asyncLoad;
	std::future<bool> asyncLoadAnim;

	void LoadModel(GameObject* gameObject);
	void LoadAnimation(GameObject* gameObject, std::string filepath);
	void LoadAnimations(std::vector<GameObject*>& gameObjects);

	ID3D11UnorderedAccessView* _UAV;
	void CreateUAV(ID3D11UnorderedAccessView* UAV, ID3D11Texture2D* m_renderTargetTexture);


	float lerp(float a, float b, float f);

	void PBRInit(std::vector<GameObject*>& gameObjects,Camera& camera);

	std::vector < std::future<bool>> createAsyncArr;
	std::future<void> addObjAsync;
	std::vector<std::thread> t;



	Texture noTextureAlbedo;
	Texture noTextureNormal;
	Texture noTextureMetallic;
	Texture noTextureRoughness;

	//std::map<Texture, std::string> globalTextureStorageAlbedo;
	//std::map<Texture, std::string> globalTextureStorageNormal;
	//std::map<Texture, std::string> globalTextureStorageRoughness;
	//std::map<Texture, std::string> globalTextureStorageMetallic;

	std::vector< std::vector<Texture>> globalTextureStorage;
	std::vector<std::string> globalTextureStorageObjFilePath;






	struct CopiedObjectData {
		std::string FilePath;
		std::vector<std::string> AnimFilePaths;
		bool isTransparent;
		bool isAnimated;
		bool isAi;
		bool attachToCamera;
		bool isPBR;
		bool isTextured;
		bool isEmptyTarget;
		physx::PxReal mass;
		XMFLOAT3 pos;
		XMFLOAT3 scale;
		XMFLOAT3 rot;
		XMFLOAT3 translateModel;
		physx::PxReal radius;
		bool frustumEnable;
		XMFLOAT3 translateObj;
		XMFLOAT4 collisionRot;
		physx::PxVec3 collisionScale;
		XMFLOAT3 frustumScale;
		Texture tex[4];
		PhysXObject::PhysXShapeType objType;
		int convexMeshDetail;
		int triangleMeshStride;
		bool isCharacter;
		bool isWalkable;
		bool isObstacle;
	};
	CopiedObjectData copiedObjectData;


	int fpsCounter = 0;


	int listbox_light_current = 0;
	std::vector< const char*> lightNames;




	GameObject skydomeObject;
	ParticleObject crosshair;
	ParticleObject muzzleFlash1, muzzleFlash2;


	bool copyObject = false;
	bool pasteObject = false;
	bool dataCopied = false;
	bool canCopy = false;
	bool canPaste = false;

	//SaveSystem saveSystem;

private:

	UIClass ui;
	imgui_addons::ImGuiFileBrowser file_dialog;

public:
	
};
