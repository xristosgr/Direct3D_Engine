#include "Game.h"

Game::Game()
{

}

void Game::RunGame(float& dt,std::vector<GameObject*>& gameObjects,physx::PxControllerManager& manager, KeyboardClass& keyboard, MouseClass& mouse,Camera& camera,bool& runPhysicsSimulation)
{
	if (runPhysicsSimulation)
	{
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			
			if(!gameObjects[i]->model.deleted)
			{
			
				if (gameObjects[i]->isAttachedToObj)
				{
					if (!gameObjects[i]->isAttached)
					{
						gameObjects[i]->isAttached = true;
						attachedToPlayer.push_back(gameObjects[i]);
					}
						
				}
				if (gameObjects[i]->isPlayer)
				{
					player = gameObjects[i];
					//playerCharacter.MouseMovement(dt, *gameObjects[i], keyboard, mouse, camera);
					//playerCharacter.Movement(dt, *gameObjects[i], attachedToPlayer, manager, keyboard, mouse, camera);
					////testCharacter.Movement(dt, *gameObjects[i], keyboard, mouse, camera);
					//playerCharacter.Actions(dt, *gameObjects[i], keyboard, mouse, camera);
					//playerCharacter.UpdateSounds();


				}

				if (gameObjects[i]->isAI)
				{
					navMeshes[i].CalculatePath(dt, gameObjects[i], player, manager, enemyController,grid);
				}

				if (gameObjects[i]->attachToCamera)
				{
					if (player->isMovingFront || player->isMovingRight)
						gameObjects[i]->model.SetAnimIndex(0, false, 0.5);

					if (!player->isMovingFront && !player->isMovingRight)
						gameObjects[i]->model.SetAnimIndex(1, true, 0.3);
					gameObjects[i]->model.Update();
				}
			}
			if (player)
			{
				playerCharacter.MouseMovement(dt, *player, keyboard, mouse, camera);
				playerCharacter.Movement(dt, *player, attachedToPlayer, manager, keyboard, mouse, camera);
				playerCharacter.Actions(dt, *player, keyboard, mouse, camera);
				playerCharacter.UpdateSounds();
			}
			if (playerCharacter.isFiring)
			{
				//playSoundAsync = std::async(std::launch::async, &SoundSystem::Play, &sound);
				//sound.Play();
			
				if (playerCharacter.canFire)
				{
			
					playerCharacter.canFire = false;
				}
			
			}
			else
			{
				playerCharacter.canFire = true;
			}

			//for (int i = 0; i < gameObjects.size(); ++i)
			//{
			//	//if (gameObjects[i]->isPlayer)
			//	//{
			//	//	player = gameObjects[i];
			//	//}
			//	if (gameObjects[i]->isAI)
			//	{
			//		navMeshes[i].CalculatePath(dt, gameObjects[i], player, manager, enemyController,grid);
			//	}
			//
			//	if (gameObjects[i]->attachToCamera)
			//	{
			//		if (player->isMovingFront || player->isMovingRight)
			//			gameObjects[i]->model.SetAnimIndex(0, false, 0.5);
			//
			//		if (!player->isMovingFront && !player->isMovingRight)
			//			gameObjects[i]->model.SetAnimIndex(1, true, 0.3);
			//		gameObjects[i]->model.Update();
			//	}
			//}

		}
		
	}
	
}
