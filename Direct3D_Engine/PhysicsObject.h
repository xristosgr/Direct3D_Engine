#pragma once

#include "ModelLoader.h"
#include "Camera.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btShapeHull.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>


struct bulletObject
{
	int id;
	bool hit;
	btRigidBody* body;
	bulletObject(btRigidBody* b, int i) : body(b), id(i), hit(false) {};
};

class PhysicsObject
{

public:
	btRigidBody* createCube(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);
	btRigidBody* createPlane(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);
	btRigidBody* createSphere(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);
	btRigidBody* createConvexShape(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);
	btRigidBody* createCapsule(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);
	btRigidBody* createHeightField(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies,int& width, int& height);
	btRigidBody* CreateTerrainShape(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies, int& width, int& height,XMFLOAT3* v1, XMFLOAT3* v2, XMFLOAT3* v3, XMFLOAT3* v4, XMFLOAT3* v5, XMFLOAT3* v6, UINT* index1, UINT* index2, UINT* index3, UINT* index4);
	btRigidBody* createImageHeightField(btDiscreteDynamicsWorld* dynamicWorld, btAlignedObjectArray<btCollisionShape*>& collisionShapes, btAlignedObjectArray<bulletObject*>& bodies);
	void PhysicsUpdate(bool& runPhysicsSimulation);

	float mass = 0.0f;
	btVector3 collisionPos;
	DirectX::XMFLOAT4 collisionRot;

	ModelLoader model;
	XMFLOAT3 translateObj;
	btRigidBody* body;


	XMFLOAT3 pos;
	btTransform t;

	float CharacterRotY = 0.0f;

	XMFLOAT3 scale;
protected:
	btDefaultMotionState* motion;






	btCollisionShape* collisionShape;
	btSphereShape* collisionShapeSphere;

	btConvexHullShape* convexShape;
	btConvexHullShape* simplifiedConvexHullShape;


	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotVector;
	DirectX::XMVECTOR posVector;






public:
	XMFLOAT3 rot;
	XMFLOAT3 prevScale;
	float radius = 1.0f;
	float prevRadius;
	XMFLOAT3 collisionScale;


	//float* terrainVals;
	//int width = 632;
	//int length = 632;

	btHeightfieldTerrainShape* heightFieldShape;
private:
	int upIndex = 1;

	float sum = 0;
	
	bool bodyInitialized = false;

	
	//long double waveSpeed = 0;
public:
	//float waveOffset = 0.002f;
	//float waveHeight = 1.0;
	//float waveDistance = 67.0;

	XMFLOAT3 finalPos;
	bool runPhysicsSimulation = false;
	bool physicsSync = false;

	bool updateConvexShape = false;

};

