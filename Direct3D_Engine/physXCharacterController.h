#pragma once
#include <PxPhysicsAPI.h>
#include <string>
class physXCharacterController
{
public:

	physXCharacterController();

	void CreateCharacterController(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxControllerManager& manager, physx::PxExtendedVec3 pos, const char* name);
	void UpdateController(physx::PxVec3& disp, float& dt);

};

