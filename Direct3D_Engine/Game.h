#pragma once
#include "EngineVariables.h"
#include "PlayerCharacter.h"
#include "FpsController.h"
#include "SoundSystem.h"
#include "AIController.h"
#include <future>
class Game : virtual public EngineVariables
{
public:
	Game();

	void RunGame(float& dt, std::vector<GameObject*>& gameObjects, physx::PxControllerManager& manager, KeyboardClass& keyboard,MouseClass& mouse,Camera& camera, bool& runPhysicsSimulation);

	//SoundSystem sound;

	//PlayerCharacter playerCharacter;
	FpsController playerCharacter;
	AIController enemyController;

	std::vector< NavMeshClass >navMeshes;
	//NavMeshClass navMesh1;
	//NavMeshClass navMesh2;
	GridClass grid;
	GameObject* player = nullptr;
	std::vector<GameObject*> attachedToPlayer;

private:
	std::future<void> move_async;

};

