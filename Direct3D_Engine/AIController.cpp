#include "AIController.h"

AIController::AIController()
{

}

void AIController::MoveTo(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager)
{
	if (!end || !start)
		return;

	_force = 0.0018f*dt;
	radius = 8.0f;



	//for (int i = 0; i < manager.getNbControllers(); ++i)
	//{
	//	if (manager.getController(i)->getActor()->getName() == start->objectName)
	//	{
	//		v1 = manager.getController(i)->getPosition();
	//
	//		start->pos = XMFLOAT3((float)manager.getController(i)->getPosition().x, (float)manager.getController(i)->getPosition().y, (float)manager.getController(i)->getPosition().z);
	//		start->phsxObj.trans = manager.getController(i)->getActor()->getGlobalPose();
	//
	//		//trans.q = physx::PxQuat((camera.rot.y), physx::PxVec3(0, 1, 0));
	//	}
	//	else if (manager.getController(i)->getActor()->getName() == end->objectName)
	//	{
	//		v2 = manager.getController(i)->getPosition();
	//	}
	//}
	//physx::PxVec3 moveVec = MoveDirectlyTo(dt, v1, v2);
	physx::PxVec3 moveVec = Move(dt, start);

	float angle = atan2(start->locToMove.x - v1.x, start->locToMove.z - v1.z);
	start->phsxObj.trans.q = physx::PxQuat((angle), physx::PxVec3(0, 1, 0));
	start->phsxObj.trans.q = start->phsxObj.trans.q.getNormalized();

	start->matrix_rotate = XMMatrixRotationRollPitchYaw(start->rot.x, start->rot.y, start->rot.z);

	start->matrix_rotate *= XMMatrixRotationAxis(XMVECTOR{ 0,1, 0 }, angle);

	for (int i = 0; i < manager.getNbControllers(); ++i)
	{
		if (manager.getController(i)->getActor()->getName() == start->objectName)
		{

			//OutputDebugStringA("CHAR!!!!\n");
			//OutputDebugStringA(("X = " + std::to_string(manager.getController(i)->getActor()->getGlobalPose().q.x)).c_str());
			//OutputDebugStringA((" Y = " + std::to_string(manager.getController(i)->getActor()->getGlobalPose().q.y)).c_str());
			//OutputDebugStringA((" Z = " + std::to_string(manager.getController(i)->getActor()->getGlobalPose().q.z) + "\n").c_str());
			manager.getController(i)->getActor()->setGlobalPose(start->phsxObj.trans);


			physx::PxControllerCollisionFlags flag;
			flag = manager.getController(i)->move(moveVec, 0.000001f, dt, NULL);

		}
	}

	if (moveVec.x != 0 || moveVec.z != 0)
	{
		start->model.SetAnimIndex(1, true, 0.7);
	}
	else
	{
		start->m_index = 0;
		start->model.SetAnimIndex(0, true, 0.5);
	}
	start->model.Update();

}

physx::PxVec3 AIController::MoveDirectlyTo(float& dt, physx::PxExtendedVec3& v1, physx::PxExtendedVec3& v2)
{


	physx::PxVec3 force = (v2 - v1) * _force;
	
	physx::PxVec3 diff = v1 - v2;
	
	float dist = diff.dot(diff);
	//OutputDebugStringA(("DIST = " + std::to_string(dist) + "\n").c_str());
	if (dist > radius)
	{
		//start->body->activate();
		return physx::PxVec3(force.x, -0.012f * dt, force.z);
	}
	else
	{
		return physx::PxVec3(0.0f, -0.012f * dt, 0.0f);
	}
}

physx::PxVec3 AIController::Move(float& dt, GameObject* start)
{
	physx::PxVec3 startLoc = physx::PxVec3(v1.x, v1.y, v1.z);
	physx::PxVec3 force = (start->locToMove - startLoc) * _force;

	physx::PxVec3 diff = v1 - v2;

	float dist = diff.dot(diff);

	if (dist > radius)
	{
		return physx::PxVec3(force.x, -0.012f * dt, force.z);
	}
	else
	{
		return physx::PxVec3(0.0f, -0.012f * dt, 0.0f);
	}
	//OutputDebugStringA(("locToMove.x = " + std::to_string(locToMove.x) + "\t").c_str());
	//OutputDebugStringA(("locToMove.z = " + std::to_string(locToMove.z) + "\n").c_str());

}
