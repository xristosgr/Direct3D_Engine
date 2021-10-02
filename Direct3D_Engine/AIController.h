#pragma once
#include "GameObject.h"
#include <PxPhysicsAPI.h>

class AIController
{
public:
	AIController();
	void MoveTo(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager);



	physx::PxExtendedVec3 v1;
	physx::PxExtendedVec3 v2;



private:

	physx::PxVec3 MoveDirectlyTo(float& dt, physx::PxExtendedVec3& v1, physx::PxExtendedVec3& v2);
	physx::PxVec3 Move(float& dt,GameObject* start);

	float _force = 0.5f;
	float radius = 15.0f;

public:

};

