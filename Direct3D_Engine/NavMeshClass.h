#pragma once
#include "GridClass.h"
#include<future>
#include "AIController.h"

class NavMeshClass
{
public:
	NavMeshClass();
	void Initialize();
	void CalculatePath(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager, AIController& controller, GridClass& grid);
	void Solve_AStar(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager, AIController& controller, GridClass& grid);


	void DrawNavMesh(Camera& camera);
	

	//GridClass grid;

private:
	void findStartEnd(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager, AIController& controller, GridClass& grid);
	float Vec3Distance(NodeClass& node1, NodeClass& node2);
	float Vec3Distance(physx::PxVec3& location, NodeClass& node2);

	std::thread t_find;
	std::future<void> find_async;
	std::future<void> solve_async;
	NodeClass* startNode;
	NodeClass* endNode;
	bool init_aStar = false;

	AppTimer timer;

public:

	std::vector<NodeClass> validNodes;

};
