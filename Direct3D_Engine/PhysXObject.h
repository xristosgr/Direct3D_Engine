#pragma once
#include <PxPhysicsAPI.h>
#include "physXCharacterController.h"
#include "Vertex.h"
#include <vector>
#include <Windows.h>
class PhysXObject
{
public:

	PhysXObject();
	void CreateCapsule(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos);
	void CreateSphere(physx::PxPhysics& physics, physx::PxScene& scene,physx::PxReal& radius, physx::PxVec3 _pos);
	void CreateCube(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos);
	void CreatePlane(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3 _pos);
	void CreateConvex(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking,std::vector<Vertex>&vertices, physx::PxVec3& _scale, physx::PxVec3 _pos);
	void CreateTriangleMesh(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, physx::PxVec3& _scale, physx::PxVec3 _pos, physx::PxQuat& _rot);
	void CreateDoor(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos);
	void CreateJoint(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos);

	void UpdateShape(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, physx::PxControllerManager& manager, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, physx::PxVec3& _scale, physx::PxReal& _radius, physx::PxVec3 _pos, physx::PxQuat& _rot);
public:

	physx::PxRigidDynamic* aActor = nullptr;
	physx::PxRigidStatic* aStaticActor = nullptr;
	physx::PxD6Joint* joint = nullptr; 
	physx::PxRigidStatic* jointActor = nullptr;
	physXCharacterController characterController;


	physx::PxVec3 jointPos;
	physx::PxTransform jointTrans;

	physx::PxShape* aShape;
	physx::PxVec3 pos;
	physx::PxTransform trans;
	physx::PxVec3 updatePos;
	physx::PxQuat rot;
	physx::PxQuat updateRot;
	physx::PxVec3 scale;
	physx::PxReal radius;
	physx::PxReal mass;
	int triangleMeshStride;
	int convexMeshDetail;
	enum PhysXShapeType
	{
		eSPHERE = 0,
		ePLANE = 1,
		eCAPSULE = 2,
		eBOX = 3,
		eCONVEXMESH = 4,
		eTRIANGLEMESH = 5,
		eDoor = 6,
		//eHEIGHTFIELD = 7,
	
		eGEOMETRY_COUNT = 8,	//!< internal use only!
		eINVALID = -1		//!< internal use only!
	};
	PhysXShapeType physXShapeType;

	bool bUpdateShape = false;
	bool isCharacter = false;
	bool isKinematic = false;
	bool isControllerCreated = false;
	bool isDxDataType = false;
	bool bCreateDoor = false;
private:

	
	physx::PxMaterial* aMaterial;
	float density;
};

