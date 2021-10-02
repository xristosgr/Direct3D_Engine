#include "NavMeshClass.h"
#include <list>

NavMeshClass::NavMeshClass()
{
	startNode = nullptr;
	endNode = nullptr;
	timer.Start();
}

void NavMeshClass::Initialize()
{
}

void NavMeshClass::CalculatePath(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager, AIController& controller, GridClass& grid)
{
	for (int i = 0; i < manager.getNbControllers(); ++i)
	{
		if (manager.getController(i)->getActor()->getName() == start->objectName)
		{
			controller.v1 = manager.getController(i)->getPosition();

			start->pos = XMFLOAT3((float)manager.getController(i)->getPosition().x, (float)manager.getController(i)->getPosition().y, (float)manager.getController(i)->getPosition().z);
			start->phsxObj.trans = manager.getController(i)->getActor()->getGlobalPose();

			//trans.q = physx::PxQuat((camera.rot.y), physx::PxVec3(0, 1, 0));
		}
		else if (manager.getController(i)->getActor()->getName() == end->objectName)
		{
			controller.v2 = manager.getController(i)->getPosition();
		}
	}

	if (!init_aStar || controller.v2.x < endNode->pos.x - 0.5 || controller.v2.x > endNode->pos.x + 0.5 || controller.v2.z < endNode->pos.z - 0.5 || controller.v2.z > endNode->pos.z + 0.5)
	{
		start->locations.clear();
		start->m_index = 0;
	
		
		for (int i = 0; i < validNodes.size(); ++i)
		{
				validNodes[i].fCost = INFINITY;
				validNodes[i].gCost = INFINITY;
				validNodes[i].hCost = INFINITY;

				validNodes[i].isVisited = false;
				validNodes[i].parent = nullptr;
				

				if (controller.v1.x >= validNodes[i].pos.x - 0.5 && controller.v1.x <= validNodes[i].pos.x + 0.5 &&
					controller.v1.y >= validNodes[i].pos.y - 2 && controller.v1.y <= validNodes[i].pos.y + 2 &&
					controller.v1.z >= validNodes[i].pos.z - 0.5 && controller.v1.z <= validNodes[i].pos.z + 0.5)
				{
					startNode = &validNodes[i];
					startNode->isActive = true;
				}
				else
				{
					grid.nodes[i].isActive = false;
				}
				if (controller.v2.x >= validNodes[i].pos.x - 0.5 && controller.v2.x <= validNodes[i].pos.x + 0.5 &&
					controller.v2.y >= validNodes[i].pos.y - 2 && controller.v2.y <= validNodes[i].pos.y + 2 &&
					controller.v2.z >= validNodes[i].pos.z - 0.5 && controller.v2.z <= validNodes[i].pos.z + 0.5)
				{
					endNode = &validNodes[i];
				}
		}

		//find_async = std::async(std::launch::async, &NavMeshClass::findStartEnd, this, std::ref(dt), start, end, std::ref(manager), std::ref(controller), std::ref(grid));
		if (!startNode || !endNode)
			return;

		
		solve_async = std::async(std::launch::async, &NavMeshClass::Solve_AStar, this, std::ref(dt), start,end, std::ref(manager), std::ref(controller),std::ref(grid));
		//Solve_AStar(dt, start, end, manager, controller, grid);
		NodeClass* p = endNode;
		while (p->parent)
		{
			p = p->parent;
		}
	
		init_aStar = true;
	}

	if (!start->locations.empty())
	{
		for (int i = 0; i < start->locations.size(); ++i)
		{
														   
			if (controller.v1.x >= start->locations[i].x -2.5 && controller.v1.x <= start->locations[i].x + 2.5 &&
				controller.v1.z >= start->locations[i].z -2.5 && controller.v1.z <= start->locations[i].z + 2.5)
			{
				//OutputDebugStringA("NEXT\n");
	
				start->m_index = i;
			}
		}
		start->locToMove = start->locations[start->m_index];
	}
	
	//OutputDebugStringA(("Index = " + std::to_string(start->m_index) + "\n").c_str());
	controller.MoveTo(dt, start, end, manager);
}

void NavMeshClass::Solve_AStar(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager, AIController& controller, GridClass& grid)
{
	NodeClass* currentNode = startNode;
	currentNode->gCost = Vec3Distance(*currentNode, *startNode);
	currentNode->hCost = Vec3Distance(*currentNode, *endNode);
	currentNode->fCost = currentNode->gCost + currentNode->hCost;
	
	std::list<NodeClass*> listNodes;
	listNodes.push_back(startNode);

	while (!listNodes.empty() && currentNode != endNode)
	{
		listNodes.sort([](const NodeClass* lhs, const NodeClass* rhs) {return lhs->fCost < rhs->fCost || lhs->fCost == rhs->fCost && lhs->hCost < rhs->hCost; });
		start->locations.push_back(physx::PxVec3(listNodes.front()->pos.x, listNodes.front()->pos.y, listNodes.front()->pos.z));
		
		while (!listNodes.empty() && listNodes.front()->isVisited)
		{
			listNodes.pop_front();
		}
	
		if (listNodes.empty())
			break;
	
		currentNode = listNodes.front();
		currentNode->isVisited = true;
	
		for (auto neighbour : currentNode->neighbours)
		{
			neighbour->gCost = Vec3Distance(*neighbour, *currentNode);
			neighbour->hCost = Vec3Distance(*neighbour, *endNode);
			neighbour->fCost = neighbour->gCost + neighbour->hCost;

			if (!neighbour->isVisited && neighbour->isValidPath)
			{
				listNodes.push_back(neighbour);
			}
			if (neighbour->fCost < currentNode->fCost || neighbour->fCost == currentNode->fCost && neighbour->hCost < currentNode->hCost)
			{
				neighbour->parent = currentNode;
			}
		}
	}
}

void NavMeshClass::DrawNavMesh(Camera& camera)
{
	//if (grid.showGrid)
	//{
	//	grid.DrawGrid(camera);
	//}
}

void NavMeshClass::findStartEnd(float& dt, GameObject* start, GameObject* end, physx::PxControllerManager& manager, AIController& controller, GridClass& grid)
{
	for (int i = 0; i < grid.nodes.size(); ++i)
	{
		grid.nodes[i].fCost = INFINITY;
		grid.nodes[i].gCost = INFINITY;
		grid.nodes[i].hCost = INFINITY;

		grid.nodes[i].isVisited = false;
		grid.nodes[i].parent = nullptr;
		if (grid.nodes[i].isValidPath)
		{

			if (controller.v1.x >= grid.nodes[i].pos.x - 0.5 && controller.v1.x <= grid.nodes[i].pos.x + 0.5 &&
				controller.v1.y >= grid.nodes[i].pos.y - 3 && controller.v1.y <= grid.nodes[i].pos.y + 3 &&
				controller.v1.z >= grid.nodes[i].pos.z - 0.5 && controller.v1.z <= grid.nodes[i].pos.z + 0.5)
			{
				startNode = &grid.nodes[i];
				startNode->isActive = true;
			}
			else
			{
				grid.nodes[i].isActive = false;
			}
			if (controller.v2.x >= grid.nodes[i].pos.x - 1 && controller.v2.x <= grid.nodes[i].pos.x + 1 &&
				controller.v2.y >= grid.nodes[i].pos.y - 3 && controller.v2.y <= grid.nodes[i].pos.y + 3 &&
				controller.v2.z >= grid.nodes[i].pos.z - 1 && controller.v2.z <= grid.nodes[i].pos.z + 1)
			{
				endNode = &grid.nodes[i];
			}
		}
	}
}

float NavMeshClass::Vec3Distance(NodeClass& node1, NodeClass& node2)
{
	physx::PxVec3 a = physx::PxVec3(node1.pos.x, node1.pos.y, node1.pos.z);
	physx::PxVec3 b = physx::PxVec3(node2.pos.x, node2.pos.y, node2.pos.z);
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z));
}

float NavMeshClass::Vec3Distance(physx::PxVec3& location, NodeClass& node2)
{
	physx::PxVec3 a = physx::PxVec3(location.x, location.y, location.z);
	physx::PxVec3 b = physx::PxVec3(node2.pos.x, node2.pos.y, node2.pos.z);
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.z - b.z) * (a.z - b.z));
}
