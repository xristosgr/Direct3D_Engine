#include "PhysXObject.h"
#include <Windows.h>
#include <string>
#include "ErrorLogger.h"
PhysXObject::PhysXObject()
{
	pos = physx::PxVec3(0, 0, 0);
	scale = physx::PxVec3(1, 1, 1);
	density = 1.0f;
	updatePos = physx::PxVec3(0, 0, 0);
	updateRot = physx::PxQuat(0,0,0,0);
	rot = physx::PxQuat(0, 0, 0, 1);
	mass = 1.0f;
	triangleMeshStride = 3;
	convexMeshDetail = 10;
}

void PhysXObject::CreateCapsule(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos)
{
	scale = _scale;

	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0, 0, 1)));

		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);
		aActor->setMass(mass);
		//float radius = 1.0f;
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor,
			physx::PxCapsuleGeometry(scale.x, scale.y / 2.0f), *aMaterial);

		aShape->setLocalPose(relativePose);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);
	}
	else
	{
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);
		aStaticActor = physx::PxCreateStatic(physics, physx::PxTransform(pos), physx::PxCapsuleGeometry(scale.x,scale.y/2), *aMaterial);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);
	}
}

void PhysXObject::CreateSphere(physx::PxPhysics& physics, physx::PxScene& scene,physx::PxReal& radius, physx::PxVec3 _pos)
{
	//this->radius = radius;

	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		physx::PxTransform relativePose(physx::PxQuat(physx::PxPi, physx::PxVec3(0, 0, 1)));
		aActor->setMass(mass);
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);
		//float radius = 1.0f;
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxSphereGeometry(radius), *aMaterial);
		aShape->setLocalPose(relativePose);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);
	}
	else
	{
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);
		aStaticActor = physx::PxCreateStatic(physics, physx::PxTransform(pos), physx::PxSphereGeometry(radius), *aMaterial);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);
	}
	//OutputDebugStringA((std::to_string(aShape->getGeometryType()) + "\n").c_str());
}

void PhysXObject::CreateCube(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos)
{
	//scale = _scale
	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		
		aActor->setMass(mass);
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);

		float radius = 1.0f;
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxBoxGeometry(_scale.x, _scale.y, _scale.z), *aMaterial);
		//aShape->setLocalPose(relativePose);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);

	}
	else
	{
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);
		aStaticActor = physx::PxCreateStatic(physics, physx::PxTransform(pos), physx::PxBoxGeometry(scale.x, scale.y, scale.z), *aMaterial);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);
	}
	
	

}

void PhysXObject::CreatePlane(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3 _pos)
{
	aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);

	aStaticActor = physx::PxCreatePlane(physics, physx::PxPlane(_pos,physx::PxVec3(0,1,0)), *aMaterial);

	aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
	aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


	trans = aStaticActor->getGlobalPose();
	trans.p = _pos;
	aStaticActor->setGlobalPose(trans);

	scene.addActor(*aStaticActor);
}

void PhysXObject::CreateConvex(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, std::vector<Vertex>& vertices, physx::PxVec3& _scale, physx::PxVec3 _pos)
{
	//scale = _scale;

	if (mass > 0)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);

		std::vector< physx::PxVec3> convexVerts;

		if (vertices.size() <= 0)
			return;
		for (int i = 0; i < vertices.size(); ++i)
		{
			convexVerts.push_back(physx::PxVec3(vertices[i].pos.x * _scale.x, vertices[i].pos.y * _scale.y, vertices[i].pos.z * _scale.z));
		}
		

		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = convexVerts.size();
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = convexVerts.data();
		convexDesc.vertexLimit = convexMeshDetail;
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;
		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;
		if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			return;
		}
		//PX_ASSERT(result);

		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = physics.createConvexMesh(input);

		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxConvexMeshGeometry(convexMesh), *aMaterial);

		aActor->setMass(mass);

		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


	}
	else
	{
		aStaticActor = physics.createRigidStatic(physx::PxTransform(_pos));
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);

		std::vector< physx::PxVec3> convexVerts;

		if (vertices.size() <= 0)
			return;
		for (int i = 0; i < vertices.size(); ++i)
		{
			convexVerts.push_back(physx::PxVec3(vertices[i].pos.x * _scale.x, vertices[i].pos.y * _scale.y, vertices[i].pos.z * _scale.z));
		}
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count = convexVerts.size();
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data = convexVerts.data();
		convexDesc.vertexLimit = convexMeshDetail;
	
		convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;
		if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			return;
		}
		//PX_ASSERT(result);
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = physics.createConvexMesh(input);

		aShape = physx::PxRigidActorExt::createExclusiveShape(*aStaticActor, physx::PxConvexMeshGeometry(convexMesh), *aMaterial);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans); 

		scene.addActor(*aStaticActor);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
	}
	

}

void PhysXObject::CreateTriangleMesh(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, physx::PxVec3& _scale, physx::PxVec3 _pos,physx::PxQuat& _rot)
{
		scale = _scale;
		rot = _rot;
		if (vertices.size() <= 0)
			return;
		aStaticActor = physics.createRigidStatic(physx::PxTransform(_pos, rot));
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.9f);

		std::vector< physx::PxU32>tris;
		std::vector<physx::PxVec3>convexVerts;

		for (int i = 0; i < vertices.size(); ++i)
		{
			convexVerts.push_back(physx::PxVec3(vertices[i].pos.x * _scale.x, vertices[i].pos.y * _scale.y, vertices[i].pos.z * _scale.z));

			if (vertices[i].pos.x * _scale.x != convexVerts[i].x)
			{
				//convexVerts.pop_back();
				//convexVerts[i].x = vertices[i].pos.x * _scale.x;
				OutputDebugStringA("physx::PxVec3 = ");
				OutputDebugStringA(std::to_string(convexVerts[i].x).c_str());
				OutputDebugStringA("\n");

				OutputDebugStringA("XMFLOAT3 = ");
				OutputDebugStringA(std::to_string(vertices[i].pos.x * _scale.x).c_str());
				OutputDebugStringA("\n");
			}
		
		}


		for (int i = 0; i < indices.size(); ++i)
		{
			tris.push_back(indices[i]);

			if (tris[i] != indices[i])
			{
				OutputDebugStringA("physx::PxUINT = ");
				OutputDebugStringA(std::to_string(tris[i]).c_str());
				OutputDebugStringA("\n");

				OutputDebugStringA("XMUINT = ");
				OutputDebugStringA(std::to_string(indices[i]).c_str());
				OutputDebugStringA("\n");
			}
		}

		physx::PxTriangleMeshDesc meshDesc;

		meshDesc.points.count = convexVerts.size();
		meshDesc.points.stride = sizeof(physx::PxVec3);
		meshDesc.points.data = convexVerts.data();

		meshDesc.triangles.count = tris.size();
		meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
		meshDesc.triangles.data = tris.data();

		physx::PxDefaultMemoryOutputStream writeBuffer;
		physx::PxTriangleMeshCookingResult::Enum result;

		bool status = cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);



		PX_ASSERT(result);
		PX_ASSERT(status);

		physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

		physx::PxTriangleMesh* triMesh = physics.createTriangleMesh(readBuffer);

		aShape = physx::PxRigidActorExt::createExclusiveShape(*aStaticActor, physx::PxTriangleMeshGeometry(triMesh), *aMaterial);


		trans = aStaticActor->getGlobalPose();
		trans.p = _pos;
		aStaticActor->setGlobalPose(trans);

		scene.addActor(*aStaticActor);

		aStaticActor->getShapes(&aShape, aStaticActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);
	


	
}

void PhysXObject::CreateDoor(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos)
{
	//scale = _scale
	if (mass > 0.0f)
	{
		aActor = physics.createRigidDynamic(physx::PxTransform(_pos));

		aActor->setMass(mass);
		aMaterial = physics.createMaterial(1.0f, 1.0f, 0.1f);

		float radius = 1.0f;
		aShape = physx::PxRigidActorExt::createExclusiveShape(*aActor, physx::PxBoxGeometry(_scale.x, _scale.y, _scale.z), *aMaterial);
		//aShape->setLocalPose(relativePose);

		aActor->setLinearDamping(0.6f);
		
		physx::PxRigidBodyExt::updateMassAndInertia(*aActor, density);

		aActor->getShapes(&aShape, aActor->getNbShapes());
		aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);


		trans = aActor->getGlobalPose();
		trans.p = _pos;
		aActor->setGlobalPose(trans);

		scene.addActor(*aActor);

	}
}

void PhysXObject::CreateJoint(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxVec3& _scale, physx::PxVec3 _pos)
{

	jointActor = physx::PxCreateStatic(physics, physx::PxTransform(pos.x, pos.y, pos.z, trans.q), physx::PxBoxGeometry(0.01, 0.01, 0.01), *aMaterial);


	//physx::PxRevoluteJoint* joint;

	physx::PxQuat q = physx::PxQuat(1.5, physx::PxVec3(0, 1, 0));
	joint = physx::PxD6JointCreate(physics, jointActor, physx::PxTransform(pos.x, pos.y, pos.z ), aActor, physx::PxTransform(pos.x, pos.y, pos.z-1.0f ));


	joint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
	joint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
	joint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLIMITED);
	joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);


	physx::PxJointLinearLimit l(physx::PxTolerancesScale(), 0.0001f);

	joint->setDistanceLimit(l);
	joint->setConstraintFlag(physx::PxConstraintFlag::ePROJECTION, true);
	joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, false);

	jointActor->getShapes(&aShape, jointActor->getNbShapes());
	aShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, false);

	trans = jointActor->getGlobalPose();
	trans.p = _pos;


	jointActor->setGlobalPose(trans);


	scene.addActor(*jointActor);
}



void PhysXObject::UpdateShape(physx::PxPhysics& physics, physx::PxScene& scene, physx::PxCooking* cooking, physx::PxControllerManager& manager, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, physx::PxVec3& _scale,physx::PxReal& _radius, physx::PxVec3 _pos, physx::PxQuat& _rot)
{
	if (isCharacter)
	{
		if(aActor)
			scene.removeActor(*aActor);
		else if(aStaticActor)
			scene.removeActor(*aStaticActor);

		
		
	}
	else
	{
		if (physXShapeType == PhysXShapeType::eBOX)
		{
			if (_scale.x != scale.x || _scale.y != scale.y || _scale.z != scale.z)
			{
				scale = _scale;
				if (aActor)
					scene.removeActor(*aActor);
				else if(aStaticActor)
					scene.removeActor(*aStaticActor);
				CreateCube(physics, scene, _scale, _pos);
			}
		}
		else if (physXShapeType == PhysXShapeType::eCAPSULE)
		{
			if (_scale.x != scale.x || _scale.y != scale.y)
			{
				if (aActor)
					scene.removeActor(*aActor);
				else if (aStaticActor)
					scene.removeActor(*aStaticActor);
				scale = _scale;
				CreateCapsule(physics, scene, _scale, _pos);
			}
		}
		else if (physXShapeType == PhysXShapeType::eSPHERE)
		{
			if (_radius != radius)
			{
				radius = _radius;
				if(aActor)
					scene.removeActor(*aActor);
				else if (aStaticActor)
					scene.removeActor(*aStaticActor);
				CreateSphere(physics, scene, _radius, _pos);
			}

		}
		else if (physXShapeType == PhysXShapeType::eCONVEXMESH)
		{
			if (_scale.x != scale.x || _scale.y != scale.y || _scale.z != scale.z)
			{
				scale = _scale;
				if (aActor)
					scene.removeActor(*aActor);
				else if (aStaticActor)
					scene.removeActor(*aStaticActor);
				CreateConvex(physics, scene, cooking, vertices, _scale, _pos);
			}

		}
		else if (physXShapeType == PhysXShapeType::eTRIANGLEMESH)
		{
			if (bUpdateShape)
			{
				//if (_rot.isSane())
				//{
					if (_scale.x != scale.x || _scale.y != scale.y || _scale.z != scale.z)
					{
						if(aStaticActor)
							scene.removeActor(*aStaticActor);
						scale = _scale;
						rot = _rot;
						CreateTriangleMesh(physics, scene, cooking, vertices, indices, _scale, _pos, _rot);
					}
					

				//}
				bUpdateShape = false;
			}

		}
		else if (physXShapeType == PhysXShapeType::eDoor)
		{
			if (_scale.x != scale.x || _scale.y != scale.y || _scale.z != scale.z)
			{
				scale = _scale;
				if (aActor)
					scene.removeActor(*aActor);
				else if (aStaticActor)
					scene.removeActor(*aStaticActor);
				CreateDoor(physics, scene, _scale, _pos);
			}
			
		}
	}


	if (bCreateDoor)
	{
		if(jointActor)
			scene.removeActor(*jointActor);
		CreateJoint(physics, scene, _scale, _pos);
		bCreateDoor = false;
	}
}
