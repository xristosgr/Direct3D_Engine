#include "physXCharacterController.h"
#include "ErrorLogger.h"

physXCharacterController::physXCharacterController()
{
}

void physXCharacterController::CreateCharacterController(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxControllerManager& manager, physx::PxExtendedVec3 pos, const char* name )
{
	physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));

	auto aMaterial = physics.createMaterial(0.0f, 0.0f, 0.0f);
	physx::PxCapsuleControllerDesc controllerDesc;
	controllerDesc.position = physx::PxExtendedVec3(pos.x, pos.y, pos.z);
	controllerDesc.slopeLimit = 0.5f;
	controllerDesc.scaleCoeff = 0.8f;
	controllerDesc.contactOffset = 0.04f;
	controllerDesc.stepOffset = 0.5f;
	controllerDesc.invisibleWallHeight = 0.0f;
	controllerDesc.maxJumpHeight = 0.0f;
	controllerDesc.radius = 0.5;
	controllerDesc.height = 2.3f;
	controllerDesc.density = 10.0f;
	controllerDesc.material = aMaterial;
	controllerDesc.climbingMode = physx::PxCapsuleClimbingMode::eEASY;
	controllerDesc.upDirection = physx::PxVec3(0, 1, 0);
	controllerDesc.volumeGrowth = 1.5f;
	//controllerDesc.reportCallback = NULL;
	//controllerDesc.behaviorCallback = NULL;
	

	if (!controllerDesc.isValid())
	{
		ErrorLogger::Log("controllerDesc failed!");
		return;
	}
	physx::PxController* controller = manager.createController(controllerDesc);
	//manager.getScene().addActor(*manager.getController(0)->getActor());

	if(!controller)
		ErrorLogger::Log("controller failed!");
	
	manager.getController(manager.getNbControllers() - 1)->getActor()->setName(name);

	physx::PxShape* _shape;
	manager.getController(manager.getNbControllers()-1)->getActor()->getShapes(&_shape, manager.getController(0)->getActor()->getNbShapes());
	_shape->setLocalPose(relativePose);
	_shape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

}

void physXCharacterController::UpdateController(physx::PxVec3& disp,float& dt)
{
	//physx::PxTransform t;
	//controller->move(physx::PxVec3(1,-9.81f, 1), 0.01f, dt, NULL);
	//controller->setPosition(physx::PxExtendedVec3(disp.x, disp.y, disp.z));
	//if (controller->getActor())
	//{
	//	t = controller->getActor()->getGlobalPose();
	//
	//	t.p.x += 0.1f;
	//
	//	controller->getActor()->setGlobalPose(t);
	//}

}
