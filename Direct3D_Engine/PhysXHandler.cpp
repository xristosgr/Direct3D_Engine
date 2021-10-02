#include "PhysXHandler.h"
#include <string>
#include "ErrorLogger.h"
//#include "ShaderSettings.hlsli"
PhysXHandler::PhysXHandler()
{
	timer.Start();
}

PhysXHandler::~PhysXHandler()
{
	ShutDown();
}

void PhysXHandler::Initialize()
{
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
		ErrorLogger::Log("PxCreateFoundation failed!");

	bool recordMemoryAllocations = true;
	mPvd = PxCreatePvd(*mFoundation);
	const char* PVD_HOST = "Host";
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	if(!transport)
		ErrorLogger::Log("PxDefaultPvdSocketTransportCreate failed!");

	physx::PxTolerancesScale scale;
	scale.length = 100;        // typical length of an object
	scale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, mPvd);

	if(!mPhysics)
		ErrorLogger::Log("PxPhysics failed!");

	physx::PxCookingParams params(mPhysics->getTolerancesScale());
	//params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	//params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eWELD_VERTICES;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
	params.suppressTriangleMeshRemapTable = true;
	params.areaTestEpsilon = 0.1f;
	params.convexMeshCookingType = physx::PxConvexMeshCookingType::eQUICKHULL;
	params.meshWeldTolerance = 1.0f;
	params.planeTolerance = 0.001f;
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);
	if(!mCooking)
		ErrorLogger::Log("PxCreateCooking failed!");

	if(!PxInitExtensions(*mPhysics,mPvd))
		ErrorLogger::Log("PxInitExtensions failed!");

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	if(!sceneDesc.isValid())
		ErrorLogger::Log("PxSceneDesc failed!");

	aScene = mPhysics->createScene(sceneDesc);
	//aScene->setVisualizationCullingBox(physx::PxBounds3(physx::PxVec3(camera.pos.x - 100.0f, camera.pos.y - 100.0f, camera.pos.z - 100.0f), physx::PxVec3(camera.pos.x + 100.0f, camera.pos.y + 100.0f, camera.pos.z + 100.0f)));
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	////aScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	////aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
	////aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCULL_BOX, 2.0f);

	if(!aScene)
		ErrorLogger::Log("createScene failed!");

	manager = PxCreateControllerManager(*aScene);

	if(!manager)
		ErrorLogger::Log("PxCreateControllerManager failed!");

	
	//playerCharacterController.CreateCharacterController(*mPhysics, *aScene, *manager);
	
}

void PhysXHandler::InitializeObjects(std::vector<GameObject*>& gameObjects)
{
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->model.deleted)
		{

			if (gameObjects[i]->phsxObj.aActor)
			{
				aScene->removeActor(*gameObjects[i]->phsxObj.aActor);
				//gameObjects[i]->phsxObj.aActor->release();
			}

			if (gameObjects[i]->phsxObj.aStaticActor)
			{
				aScene->removeActor(*gameObjects[i]->phsxObj.aStaticActor);
				//gameObjects[i]->phsxObj.aStaticActor->release();
			}
			
		}
		else
		{

			if (!gameObjects[i]->phsxObj.isCharacter)
			{
				
				if (gameObjects[i]->phsxObj.physXShapeType == -1)
				{
					gameObjects[i]->constructShape = false;
				}
				else
				{
					gameObjects[i]->phsxObj.UpdateShape(*mPhysics, *aScene, mCooking, *manager, gameObjects[i]->model.vertices, gameObjects[i]->model.indices, gameObjects[i]->physxScale, gameObjects[i]->radius, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z), gameObjects[i]->phsxObj.updateRot);
				}
			}
			else
			{

				if (!gameObjects[i]->phsxObj.isControllerCreated)
				{
					gameObjects[i]->phsxObj.updatePos = physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z);
					gameObjects[i]->phsxObj.characterController.CreateCharacterController(*mPhysics, *aScene, *manager,physx::PxExtendedVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z), gameObjects[i]->objectName.c_str());
					gameObjects[i]->phsxObj.isControllerCreated = true;
				
				}

			}
			if (gameObjects[i]->constructShape)
			{
				if (gameObjects[i]->phsxObj.aActor)
				{
					aScene->removeActor(*gameObjects[i]->phsxObj.aActor);
				}

				if (gameObjects[i]->phsxObj.aStaticActor)
				{
					aScene->removeActor(*gameObjects[i]->phsxObj.aStaticActor);
				}

				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eBOX)
				{
					gameObjects[i]->phsxObj.CreateCube(*mPhysics, *aScene, gameObjects[i]->physxScale, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
				}
				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eSPHERE)
				{
					gameObjects[i]->phsxObj.CreateSphere(*mPhysics, *aScene, gameObjects[i]->radius, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
				}
				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eCAPSULE)
				{
					gameObjects[i]->phsxObj.CreateCapsule(*mPhysics, *aScene, gameObjects[i]->physxScale, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
				}
				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::ePLANE)
				{
					gameObjects[i]->phsxObj.CreatePlane(*mPhysics, *aScene, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
				}
				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eCONVEXMESH)
				{
					gameObjects[i]->phsxObj.CreateConvex(*mPhysics, *aScene, mCooking, gameObjects[i]->model.vertices, gameObjects[i]->physxScale, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
				}
				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eTRIANGLEMESH)
				{
					gameObjects[i]->phsxObj.CreateTriangleMesh(*mPhysics, *aScene, mCooking, gameObjects[i]->model.vertices, gameObjects[i]->model.indices, gameObjects[i]->physxScale, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z), gameObjects[i]->phsxObj.rot);
				}
				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eDoor)
				{
					gameObjects[i]->phsxObj.CreateDoor(*mPhysics, *aScene, gameObjects[i]->physxScale, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
					if (!gameObjects[i]->phsxObj.jointActor)
						gameObjects[i]->phsxObj.CreateJoint(*mPhysics, *aScene, gameObjects[i]->physxScale, physx::PxVec3(gameObjects[i]->pos.x, gameObjects[i]->pos.y, gameObjects[i]->pos.z));
				}
			

				//OutputDebugStringA("SHAPE CONSTRICTED!!!!!\n");
				gameObjects[i]->constructShape = false;
			}
		}
	
	}
}

void PhysXHandler::ShutDown()
{
	if (mPhysics)
		mPhysics->release();

	PxCloseExtensions();

	if (mFoundation)
		mFoundation->release();
}

void PhysXHandler::UpdatePhysX(float& dt,std::vector<GameObject*>&gameObjects, MouseClass& mouse, KeyboardClass& keyboard, bool& runPhysicsSimulation)
{
	
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->phsxObj.isControllerCreated)
		{
			if (gameObjects[i]->phsxObj.isCharacter)
			{
				for (int j = 0; j < manager->getNbControllers(); ++j)
				{
					if (manager->getController(j)->getActor()->getName() == gameObjects[i]->objectName)
					{
						gameObjects[i]->AttachController(*manager->getController(j),runPhysicsSimulation);
					}
				}
	
	
	
			}
		}
	
		
	}
	
}

void PhysXHandler::MouseRayCast(std::vector<GameObject*>& gameObjects,Camera& camera,MouseClass& mouse,KeyboardClass& keyboard, int& width, int& height, int& selectedObject, std::vector<const char*>& objNames, int& listbox_item_current)
{
	aScene->setVisualizationCullingBox(physx::PxBounds3(physx::PxVec3(camera.pos.x - 40.0f, camera.pos.y - 40.0f, camera.pos.z - 40.0f), physx::PxVec3(camera.pos.x + 40.0f, camera.pos.y + 40.0f, camera.pos.z + 40.0f)));
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	aScene->setVisualizationParameter(physx::PxVisualizationParameter::eJOINT_LIMITS, 1.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_AABBS, 2.0f);
	//aScene->setVisualizationParameter(physx::PxVisualizationParameter::eCULL_BOX, 2.0f);

	float pointX, pointY;
	DirectX::XMMATRIX viewMatrix, inverseViewMatrix;
	DirectX::XMFLOAT3 direction;

	pointX = ((2.0f * (float)mouse.GetPosX()) / (float)width) - 1.0f;
	pointY = (((2.0f * (float)mouse.GetPosY()) / (float)height) - 1.0f) * -1.0f;

	DirectX::XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
	pointX = pointX / projection._11;
	pointY = pointY / projection._22;


	viewMatrix = camera.GetViewMatrix();
	inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);

	DirectX::XMFLOAT4X4 invView;
	DirectX::XMStoreFloat4x4(&invView, inverseViewMatrix);

	direction.x = (pointX * invView._11) + (pointY * invView._21) + invView._31;
	direction.y = (pointX * invView._12) + (pointY * invView._22) + invView._32;
	direction.z = (pointX * invView._13) + (pointY * invView._23) + invView._33;


	physx::PxVec3 origin = physx::PxVec3(camera.pos.x, camera.pos.y, camera.pos.z);

	physx::PxVec3 unitDir;
	unitDir = physx::PxVec3(direction.x, direction.y, direction.z);

	unitDir.normalize();
	physx::PxReal maxDistance = 100000.0f;
	physx::PxRaycastBuffer hit;

	bool status = aScene->raycast(origin, unitDir, maxDistance, hit);

	//std::string s = "X = " + std::to_string(hit.block.position.x) + " |Y = " + std::to_string(hit.block.position.y) + " |Z = " + std::to_string(hit.block.position.z) + "\n";
	//OutputDebugStringA(s.c_str());
	



	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!gameObjects[i]->model.deleted)
		{
			if (listbox_item_current == -1)
			{
				if (gameObjects[i]->phsxObj.aActor)
				{


					gameObjects[i]->phsxObj.aActor->getShapes(&_shape, gameObjects[i]->phsxObj.aActor->getNbShapes());
					_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

					if (gameObjects[i]->phsxObj.jointActor)
					{
						gameObjects[i]->phsxObj.jointActor->getShapes(&_shape, gameObjects[i]->phsxObj.jointActor->getNbShapes());
						_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
					}
					if(gameObjects[i]->phsxObj.joint)
						gameObjects[i]->phsxObj.joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, false);
				}
				else if (gameObjects[i]->phsxObj.aStaticActor)
				{
					gameObjects[i]->phsxObj.aStaticActor->getShapes(&_shape, gameObjects[i]->phsxObj.aStaticActor->getNbShapes());
					_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
				}

				for (int j = 0; j < manager->getNbControllers(); ++j)
				{
					manager->getController(j)->getActor()->getShapes(&_shape, manager->getController(j)->getActor()->getNbShapes());
					_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
				}
			}
			else
			{
				if (!mouse.IsRightDown())
				{
					
					if (objNames[listbox_item_current] == gameObjects[i]->objectName)
					{
						//OutputDebugStringA(("NAME = " + gameObjects[i]->objectName + "\n").c_str());
						if (gameObjects[i]->phsxObj.aActor)
						{
							gameObjects[i]->phsxObj.aActor->getShapes(&_shape, gameObjects[i]->phsxObj.aActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							if (gameObjects[i]->phsxObj.jointActor)
							{
								gameObjects[i]->phsxObj.jointActor->getShapes(&_shape, gameObjects[i]->phsxObj.jointActor->getNbShapes());
								_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							}
							if (gameObjects[i]->phsxObj.joint)
								gameObjects[i]->phsxObj.joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
						}
						else if (gameObjects[i]->phsxObj.aStaticActor)
						{
							gameObjects[i]->phsxObj.aStaticActor->getShapes(&_shape, gameObjects[i]->phsxObj.aStaticActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
						}
						else
						{
							for (int j = 0; j < manager->getNbControllers(); ++j)
							{
								if (gameObjects[i]->objectName == manager->getController(j)->getActor()->getName())
								{
									manager->getController(j)->getActor()->getShapes(&_shape, manager->getController(j)->getActor()->getNbShapes());
									_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
								}
							}
						}
					
					}
					else
					{
						if (gameObjects[i]->phsxObj.aActor)
						{
							gameObjects[i]->phsxObj.aActor->getShapes(&_shape, gameObjects[i]->phsxObj.aActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
							if (gameObjects[i]->phsxObj.jointActor)
							{
								gameObjects[i]->phsxObj.jointActor->getShapes(&_shape, gameObjects[i]->phsxObj.jointActor->getNbShapes());
								_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							}
							if (gameObjects[i]->phsxObj.joint)
								gameObjects[i]->phsxObj.joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, false);
						}
						else if (gameObjects[i]->phsxObj.aStaticActor)
						{
							gameObjects[i]->phsxObj.aStaticActor->getShapes(&_shape, gameObjects[i]->phsxObj.aStaticActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
						}
						else
						{
							for (int j = 0; j < manager->getNbControllers(); ++j)
							{
								if (gameObjects[i]->objectName == manager->getController(j)->getActor()->getName())
								{
									manager->getController(j)->getActor()->getShapes(&_shape, manager->getController(j)->getActor()->getNbShapes());
									_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
								}
							}
						}
						
					}
				}
			}
		
			if (mouse.IsRightDown())
			{

				if (status)
				{
					if (gameObjects[i]->phsxObj.aActor)
					{
						if (hit.block.actor->getName() == gameObjects[i]->phsxObj.aActor->getName())
						{
							for (int j = 0; j < objNames.size(); ++j)
							{
								if (objNames[j] == gameObjects[i]->objectName)
								{
									listbox_item_current = j;
								}
							}

							gameObjects[i]->phsxObj.aActor->getShapes(&_shape, gameObjects[i]->phsxObj.aActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							if (gameObjects[i]->phsxObj.jointActor)
							{
								gameObjects[i]->phsxObj.jointActor->getShapes(&_shape, gameObjects[i]->phsxObj.jointActor->getNbShapes());
								_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							}
							if (gameObjects[i]->phsxObj.joint)
								gameObjects[i]->phsxObj.joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);

							selectedObject = i;
						}
						else
						{
							gameObjects[i]->phsxObj.aActor->getShapes(&_shape, gameObjects[i]->phsxObj.aActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
							if (gameObjects[i]->phsxObj.jointActor)
							{
								gameObjects[i]->phsxObj.jointActor->getShapes(&_shape, gameObjects[i]->phsxObj.jointActor->getNbShapes());
								_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
							}
							if (gameObjects[i]->phsxObj.joint)
								gameObjects[i]->phsxObj.joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, false);
						}
					}
					else if (gameObjects[i]->phsxObj.aStaticActor)
					{
						if (hit.block.actor->getName() == gameObjects[i]->phsxObj.aStaticActor->getName())
						{
							for (int j = 0; j < objNames.size(); ++j)
							{
								if (objNames[j] == gameObjects[i]->objectName)
								{
									listbox_item_current = j;
								}
							}
							gameObjects[i]->phsxObj.aStaticActor->getShapes(&_shape, gameObjects[i]->phsxObj.aStaticActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							selectedObject = i;
						}
						else
						{
							gameObjects[i]->phsxObj.aStaticActor->getShapes(&_shape, gameObjects[i]->phsxObj.aStaticActor->getNbShapes());
							_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
						}
					}
					else
					{
						for (int j = 0; j < manager->getNbControllers(); ++j)
						{
							if (hit.block.actor->getName() == manager->getController(j)->getActor()->getName())
							{
								for (int k = 0; k < objNames.size(); ++k)
								{
									if (objNames[k] == hit.block.actor->getName())
									{
										listbox_item_current = k;
									}
								}
								manager->getController(j)->getActor()->getShapes(&_shape, manager->getController(j)->getActor()->getNbShapes());
								_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);
							}
							else
							{
								manager->getController(j)->getActor()->getShapes(&_shape, manager->getController(j)->getActor()->getNbShapes());
								_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
							
							}
						}
					}
				
				}
			}



			if (listbox_item_current > -1)
			{
				if (gameObjects[i]->objectName == gameObjects[selectedObject]->objectName || objNames[listbox_item_current] == gameObjects[i]->objectName)
				{
					if (gameObjects[i]->mouseFollow)
					{
						if (gameObjects[i]->pos.x > hit.block.position.x)
							gameObjects[i]->phsxObj.updatePos.x -= 0.12f;
						else if (gameObjects[i]->pos.x < hit.block.position.x)
							gameObjects[i]->phsxObj.updatePos.x += 0.12f;
						else
							gameObjects[i]->phsxObj.updatePos.x = 0.0f;
						if (gameObjects[i]->pos.z > hit.block.position.z)
							gameObjects[i]->phsxObj.updatePos.z -= 0.12f;
						else if (gameObjects[i]->pos.z < hit.block.position.z)
							gameObjects[i]->phsxObj.updatePos.z += 0.12f;
						else
							gameObjects[i]->phsxObj.updatePos.z = 0.0f;

					}

					if (keyboard.KeyIsPressed('Z'))
					{
						gameObjects[i]->phsxObj.updatePos.y -= 0.1f;
					}
					else if (keyboard.KeyIsPressed('X'))
					{
						gameObjects[i]->phsxObj.updatePos.y += 0.1f;
					}
				}
			}

		}

	}

	
	hit.finalizeQuery();
}

void PhysXHandler::FallCheck(std::vector<GameObject*>& gameObjects)
{

	for (int j = 0; j < gameObjects.size(); ++j)
	{
		for (int i = 0; i < manager->getNbControllers(); ++i)
		{
			physx::PxVec3 origin = physx::PxVec3(manager->getController(i)->getFootPosition().x, manager->getController(i)->getFootPosition().y + 0.2f, manager->getController(i)->getFootPosition().z);

			physx::PxVec3 unitDir;
			unitDir = physx::PxVec3(0, -1, 0);

			unitDir.normalize();
			physx::PxReal maxDistance = 0.4f;
			physx::PxRaycastBuffer hit;

			bool status = aScene->raycast(origin, unitDir, maxDistance, hit);

			if (status)
			{
				if (gameObjects[j]->phsxObj.isCharacter)
				{
					if (gameObjects[j]->objectName == manager->getController(i)->getActor()->getName())
					{
						//if (gameObjects[j]->isPlayer)
						//{
						//	//OutputDebugStringA("COLLIDING | NAME = ");
						//	//OutputDebugStringA(manager->getController(i)->getActor()->getName());
						//	//OutputDebugStringA("\n");
						//}

						gameObjects[j]->isFalling = false;
						//OutputDebugStringA(("gameObject.isFalling = " + std::to_string(gameObjects[j]->isFalling) + "\n").c_str());
					}
				}


			}
			else
			{
				if (gameObjects[j]->phsxObj.isCharacter)
				{
					if (gameObjects[j]->objectName == manager->getController(i)->getActor()->getName())
					{
						//if (gameObjects[j]->isPlayer)
						//{
						//	//OutputDebugStringA("FALLING | NAME = ");
						//	//OutputDebugStringA(manager->getController(i)->getActor()->getName());
						//	//OutputDebugStringA("\n");
						//}

						gameObjects[j]->isFalling = true;
						
					}
				}


			}
			//if(gameObjects[j]->isPlayer)
			//	OutputDebugStringA(("gameObject.isFalling = " + std::to_string(gameObjects[j]->isFalling) + "\n").c_str());
	}
	
		//hit.finalizeQuery();
	}

}

void PhysXHandler::NavMeshRayCast(GridClass& grid, std::vector<GameObject*>& gameObjects)
{
	for (int i = 0; i < grid.nodes.size(); ++i)
	{
		physx::PxVec3 origin = physx::PxVec3(grid.nodes[i].pos.x, grid.nodes[i].pos.y, grid.nodes[i].pos.z);
		physx::PxVec3 origin1 = physx::PxVec3(grid.nodes[i].pos.x + 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z);
		physx::PxVec3 origin2 = physx::PxVec3(grid.nodes[i].pos.x - 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z);
		physx::PxVec3 origin3 = physx::PxVec3(grid.nodes[i].pos.x, grid.nodes[i].pos.y, grid.nodes[i].pos.z + 0.6);
		physx::PxVec3 origin4 = physx::PxVec3(grid.nodes[i].pos.x, grid.nodes[i].pos.y, grid.nodes[i].pos.z - 0.6);

		physx::PxVec3 origin5 = physx::PxVec3(grid.nodes[i].pos.x + 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z - 0.6);
		physx::PxVec3 origin6 = physx::PxVec3(grid.nodes[i].pos.x - 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z + 0.6);
		physx::PxVec3 origin7 = physx::PxVec3(grid.nodes[i].pos.x + 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z + 0.6);
		physx::PxVec3 origin8 = physx::PxVec3(grid.nodes[i].pos.x - 0.6, grid.nodes[i].pos.y, grid.nodes[i].pos.z - 0.6);

		physx::PxVec3 unitDir;
		unitDir = physx::PxVec3(0, -1, 0);

		unitDir.normalize();
		physx::PxReal maxDistance = 1.0f;
		physx::PxRaycastBuffer hit;
		physx::PxRaycastBuffer hit1;
		physx::PxRaycastBuffer hit2;
		physx::PxRaycastBuffer hit3;
		physx::PxRaycastBuffer hit4;

		physx::PxRaycastBuffer hit5;
		physx::PxRaycastBuffer hit6;
		physx::PxRaycastBuffer hit7;
		physx::PxRaycastBuffer hit8;

		physx::PxHitFlags _hitFlags;
		_hitFlags.set(physx::PxHitFlag::eDEFAULT);

		physx::PxQueryFilterData _QueryFilterData;
		_QueryFilterData.flags.set(physx::PxQueryFlag::eSTATIC);
		bool status = aScene->raycast(origin, unitDir, maxDistance, hit, _hitFlags, _QueryFilterData);
		bool status1 = aScene->raycast(origin1, unitDir, maxDistance, hit1, _hitFlags, _QueryFilterData);
		bool status2 = aScene->raycast(origin2, unitDir, maxDistance, hit2, _hitFlags, _QueryFilterData);
		bool status3 = aScene->raycast(origin3, unitDir, maxDistance, hit3, _hitFlags, _QueryFilterData);
		bool status4 = aScene->raycast(origin4, unitDir, maxDistance, hit4, _hitFlags, _QueryFilterData);

		bool status5 = aScene->raycast(origin5, unitDir, maxDistance, hit5, _hitFlags, _QueryFilterData);
		bool status6 = aScene->raycast(origin6, unitDir, maxDistance, hit6, _hitFlags, _QueryFilterData);
		bool status7 = aScene->raycast(origin7, unitDir, maxDistance, hit7, _hitFlags, _QueryFilterData);
		bool status8 = aScene->raycast(origin8, unitDir, maxDistance, hit8, _hitFlags, _QueryFilterData);


		for (int j = 0; j < gameObjects.size(); ++j)
		{
			if (status)
			{
				if (gameObjects[j]->phsxObj.aStaticActor)
				{
					if (hit.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
					{
						if (gameObjects[j]->isWalkable)
						{
							grid.nodes[i].isValidPath = true;
						}
						else
						{
							grid.nodes[i].isValidPath = false;
						}
						if (gameObjects[j]->isObstacle)
						{
							grid.nodes[i].isValidPath = false;
						}
					}
				}
				hit.finalizeQuery();
			}
			else
			{
				grid.nodes[i].isValidPath = false;
			}

			//if (status1)
			//{
			//	if (gameObjects[j]->phsxObj.aStaticActor)
			//	{
			//		if (hit1.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
			//		{
			//			if (gameObjects[j]->isObstacle)
			//			{
			//				grid.nodes[i].isValidPath = false;
			//			}
			//		}
			//	}
			//
			//
			//	hit1.finalizeQuery();
			//}
			//
			//
			//if (status2)
			//{
			//	if (gameObjects[j]->phsxObj.aStaticActor)
			//	{
			//		if (hit2.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
			//		{
			//			if (gameObjects[j]->isObstacle)
			//			{
			//				grid.nodes[i].isValidPath = false;
			//			}
			//		}
			//	}
			//
			//	hit2.finalizeQuery();
			//}
			//
			//if (status3)
			//{
			//	if (gameObjects[j]->phsxObj.aStaticActor)
			//	{
			//		if (hit3.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
			//		{
			//			if (gameObjects[j]->isObstacle)
			//			{
			//				grid.nodes[i].isValidPath = false;
			//			}
			//		}
			//	}
			//
			//	hit3.finalizeQuery();
			//}
			//
			//
			//if (status4)
			//{
			//	if (gameObjects[j]->phsxObj.aStaticActor)
			//	{
			//		if (hit4.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
			//		{
			//			if (gameObjects[j]->isObstacle)
			//			{
			//				grid.nodes[i].isValidPath = false;
			//			}
			//		}
			//	}
			//
			//	hit4.finalizeQuery();
			//}




			if (status5)
			{
				if (gameObjects[j]->phsxObj.aStaticActor)
				{
					if (hit5.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
					{
						if (gameObjects[j]->isObstacle)
						{
							grid.nodes[i].isValidPath = false;
						}
					}
				}

				hit5.finalizeQuery();
			}



			if (status6)
			{
				if (gameObjects[j]->phsxObj.aStaticActor)
				{
					if (hit6.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
					{
						if (gameObjects[j]->isObstacle)
						{
							grid.nodes[i].isValidPath = false;
						}
					}
				}

				hit6.finalizeQuery();
			}



			if (status7)
			{
				if (gameObjects[j]->phsxObj.aStaticActor)
				{
					if (hit7.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
					{
						if (gameObjects[j]->isObstacle)
						{
							grid.nodes[i].isValidPath = false;
						}
					}
				}


				hit7.finalizeQuery();
			}



			if (status8)
			{
				if (gameObjects[j]->phsxObj.aStaticActor)
				{
					if (hit8.block.actor->getName() == gameObjects[j]->phsxObj.aStaticActor->getName())
					{
						if (gameObjects[j]->isObstacle)
						{
							grid.nodes[i].isValidPath = false;
						}
					}
				}


				hit8.finalizeQuery();
			}
		}
	}
}
void PhysXHandler::CharacterCollisonDetection(Camera& camera, GameObject& player, std::vector<GameObject*>& gameObjects)
{

	for (int i = 0; i < manager->getNbControllers(); ++i)
	{
		DirectX::XMVECTOR default_forward_vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		physx::PxTransform trans = manager->getController(i)->getActor()->getGlobalPose();
		//physx::PxQuat rot = manager->getController(i)->getActor()->getGlobalPose().q;
				
		//OutputDebugStringA("CHAR!!!!\n");
		//OutputDebugStringA(("X = " + std::to_string(trans.q.x)).c_str());
		//OutputDebugStringA((" Y = " + std::to_string(trans.q.y)).c_str());
		//OutputDebugStringA((" Z = " + std::to_string(trans.q.z) + "\n").c_str());

		XMMATRIX matrixRot = XMMatrixRotationQuaternion(XMVectorSet(trans.q.x, trans.q.y, trans.q.z, trans.q.w));

		DirectX::XMVECTOR vec_forward = XMVector3TransformCoord(default_forward_vec, matrixRot);

		XMFLOAT4 forwardDir;
		XMStoreFloat4(&forwardDir, vec_forward);
		physx::PxVec3 unitDir;
		unitDir = physx::PxVec3(forwardDir.x, forwardDir.y, forwardDir.z);

		unitDir.normalize();

		physx::PxVec3 origin = physx::PxVec3(trans.p.x+ unitDir.x, trans.p.y+ unitDir.y, trans.p.z+ unitDir.z);
	


		physx::PxReal maxDistance = 0.5f;
		physx::PxRaycastBuffer hit;

		physx::PxHitFlags _hitFlags;
		_hitFlags.set(physx::PxHitFlag::eDEFAULT);

		physx::PxQueryFilterData _QueryFilterData;
		_QueryFilterData.flags.set(physx::PxQueryFlag::eDYNAMIC);
		bool status = aScene->raycast(origin, unitDir, maxDistance, hit, _hitFlags, _QueryFilterData);

		if (status)
		{
			for (int j = 0; j < gameObjects.size(); ++j)
			{
				if (gameObjects[j]->phsxObj.aActor)
				{
					if (hit.block.actor->getName() == gameObjects[j]->phsxObj.aActor->getName())
					{
						//OutputDebugStringA("HIT!!!!\n");
						gameObjects[j]->phsxObj.aActor->setLinearVelocity(physx::PxVec3(unitDir.x * 1.0, unitDir.y * 1.0, unitDir.z * 1.0));
						//gameObjects[i]->phsxObj.aActor->addForce(physx::PxVec3(forwardDir.x * 5, forwardDir.y * 5, forwardDir.z * 5));
					}
				}

			}
			
		}
		
	}

}

void PhysXHandler::PlayerCollisonDetection(Camera& camera, GameObject& player, std::vector<GameObject*>& gameObjects)
{
	XMFLOAT4 forwardDir;
	XMStoreFloat4(&forwardDir, camera.vec_forward);

	physx::PxVec3 origin = physx::PxVec3(player.pos.x+ forwardDir.x, player.pos.y+ forwardDir.y, player.pos.z+ forwardDir.z);
	physx::PxVec3 unitDir;

	unitDir = physx::PxVec3(forwardDir.x, forwardDir.y, forwardDir.z);
	//OutputDebugStringA("PLAYER!!!!\n");

	unitDir.normalize();
	//OutputDebugStringA(("X = " + std::to_string(unitDir.x)).c_str());
	//OutputDebugStringA((" Y = " + std::to_string(unitDir.y)).c_str());
	//OutputDebugStringA((" Z = " + std::to_string(unitDir.z) + "\n").c_str());
	physx::PxReal maxDistance = 0.5f;
	physx::PxRaycastBuffer hit;

	physx::PxHitFlags _hitFlags;
	_hitFlags.set(physx::PxHitFlag::eDEFAULT);

	physx::PxQueryFilterData _QueryFilterData;
	_QueryFilterData.flags.set(physx::PxQueryFlag::eDYNAMIC);
	bool status = aScene->raycast(origin, unitDir, maxDistance, hit, _hitFlags, _QueryFilterData);

	if (status)
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			if (gameObjects[i]->phsxObj.aActor)
			{
				if (hit.block.actor->getName() == gameObjects[i]->phsxObj.aActor->getName())
				{
					gameObjects[i]->phsxObj.aActor->setLinearVelocity(physx::PxVec3(forwardDir.x*1.4, forwardDir.y*1.4, forwardDir.z*1.4));
					//gameObjects[i]->phsxObj.aActor->addForce(physx::PxVec3(forwardDir.x * 5, forwardDir.y * 5, forwardDir.z * 5));
				}
			}
			
		}
		//OutputDebugStringA("HIT!!!!\n");
	}
}

bool PhysXHandler::advance(float& dt,int& vSync)
{
	mAccumulator = 0.0f;
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;


	if (timer.GetMilisecondsElapsed() > 1000.0f)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		//OutputDebugStringA((fpsString + "\n").c_str());
		mStepSize = 1.0f / (float)fpsCounter;
		fpsCounter = 0;
		timer.Restart();
	}
	//if (!physxInit)
	//{
	//	mStepSize = 1.0f / 60.0f;
	//	physxInit = true;
	//}
	//if (vSync == 1)
	//{
	//	mStepSize = 1.0f / 60.0f;
	//}
	//else if (vSync == 2)
	//{
	//	mStepSize = 1.0f / 30.0f;
	//}
	mAccumulator += dt;
	if (mAccumulator < mStepSize)
		return false;
	
	mAccumulator -= mStepSize;

	aScene->simulate(mStepSize);

	return true;
}
