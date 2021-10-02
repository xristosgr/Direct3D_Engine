#pragma once

#include "InputVariables.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Camera.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "ModelLoader.h"
#include "PhysXObject.h"

using namespace DirectX;

class GameObject
{

	
public:
	GameObject();
	bool Initialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool isAnimated, bool isTextured, bool isPBR, bool isEmptyTarget, bool isObjectDeleted);
	void DebugDraw(const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured);
	void Draw(const XMMATRIX& viewMatrix, const  XMMATRIX& projectionMatrix, bool isTextured = true,bool frustumCheck = true);
	virtual void DrawGui(std::string name, ID3D11DeviceContext* deviceContext,KeyboardClass& keyboard,Camera& camera);
	void SetScale(const XMVECTOR& pos);
	void SetRotation(const XMVECTOR& pos);
	void SetTranslation(const XMVECTOR& pos);

	void SetTranslation(const XMFLOAT3& pos);
	void AttachToObject(GameObject& gameObject);

	DirectX::XMVECTOR GetPositionVector();
	DirectX::XMFLOAT3 GetPositionFloat3();
	DirectX::XMFLOAT3 GetScaleFloat3();


	void SetupCamera(int windowWidth, int windowHeight);
	Camera* GetCamera();
	void UpdateCamera();

	void AttachController(physx::PxController& characterController,bool& runPhysicsSimulation);
private:
	void Input(KeyboardClass& keyBoard, MouseClass& mouse);
	bool UpdatePhysX(XMMATRIX& scale, XMMATRIX& rotate, XMMATRIX& translate);
public:
	ModelLoader model;

	DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX ReflectViewMatrix;
	XMMATRIX RefractViewMatrix;


	Camera cameraReflect;
	Camera cameraRefract;

	int curAnim = 0;

	bool isMovingFront = false;
	bool isMovingRight = false;
	bool isMovingLeft = false;
	bool isJumping = false;
	bool constructShape = false;
	bool mouseFollow = false;
	bool isInEditMode = false;
	bool isAttachedToObj = false;

	
	std::string objectName = "";





	std::vector< const char*> physXNames;
	PhysXObject phsxObj;


protected:
public:
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMFLOAT3 pos;
	XMFLOAT3 scale;
	XMFLOAT3 rot;
	
	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMVECTOR scaleVector;


	XMMATRIX matrix_scale;
	XMMATRIX matrix_rotate;
	XMMATRIX matrix_translate;

public:
	XMFLOAT3 camPos;
	XMFLOAT3 translateModel;
	XMFLOAT3  translateModelOffset;
	XMFLOAT3 frustumScale;
	physx::PxReal radius;
	physx::PxVec3 physxScale;

	std::string filePath = " ";
	bool isPBR;
	bool isTextured;
	bool isAnimated;
	bool isPlayer = false;
	bool isAI = false;
	bool isTransparent = false;

	XMFLOAT3 tempModelTranslation;
	//physics vars
	int AnimCount = 0;
	std::vector<std::string> AnimFiles;

	std::string inName;
private:

	bool windowActive = false;

	bool nameInitialized = false;
	


public:
	bool texturesLoaded = false;
	std::string tempName;
	bool frustumEnable = true;
	bool attachToCamera = false;
	bool isFiring = false;
	bool canFire = true;

	bool emptyTarget;


	bool isVisible = false;
	bool objectSelected;
	bool listBoxSelected = false;
	int ID = -1;
	bool isFalling = false;
	bool isWalkable;
	bool isObstacle;

	float rayOffset = 0.0f;

	int list_box_selected_obj = -1;
private:
	int _listbox_item_current = -1;
	std::vector< const char*> physicsNames;

public:
	GameObject* parentObj = nullptr;
	std::vector< physx::PxVec3> locations;

	unsigned int m_index = 0;

	physx::PxVec3 locToMove;
	bool initAstar = false;

	bool isAttached = false;
};

