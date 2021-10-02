#pragma once
#include <PxPhysicsAPI.h>
#include"PxDefaultErrorCallback.h"
#include"PxDefaultAllocator.h"
#include "physXCharacterController.h"
#include "PhysXObject.h"
#include "GameObject.h"
#include<vector>
#include"AppTimer.h"
#include "NavMeshClass.h"

class PhysXHandler
{
public:
	PhysXHandler();
	~PhysXHandler();

	void Initialize();
	void InitializeObjects(std::vector<GameObject*>& gameObjects);
	void ShutDown();

	void UpdatePhysX(float& dt, std::vector<GameObject*>& gameObjects, MouseClass& mouse, KeyboardClass& keyboard, bool& runPhysicsSimulation);
	void MouseRayCast(std::vector<GameObject*>& gameObjects, Camera& camera, MouseClass& mouse, KeyboardClass& keyboard, int& width, int& height, int& selectedObject,std::vector<const char*>& objNames,int& listbox_item_current);
	void FallCheck(std::vector<GameObject*>& gameObjects);
	void NavMeshRayCast(GridClass& grid, std::vector<GameObject*>& gameObjects);
	void CharacterCollisonDetection(Camera& camera, GameObject& player, std::vector<GameObject*>& gameObjects);
	void PlayerCollisonDetection(Camera& camera, GameObject& player, std::vector<GameObject*>& gameObjects);
	bool advance(float& dt, int& vSync);
private:
	physx::PxFoundation* mFoundation;
	physx::PxPvd* mPvd;
	physx::PxCooking* mCooking;
	physx::PxDefaultCpuDispatcher* gDispatcher;

	float mAccumulator = 0.0f;
	float mStepSize = 1.0f / 60.0f;

	AppTimer timer;
public:
	physx::PxPhysics* mPhysics;
	physx::PxScene* aScene;
	physx::PxControllerManager* manager;
	//std::vector<PhysXObject> physXObjects;
	//physXCharacterController playerCharacterController;


private:
	physx::PxShape* _shape;

	bool canJump = true;
	bool physxInit = false;

};

