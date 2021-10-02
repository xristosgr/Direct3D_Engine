#include "FpsController.h"


FpsController::FpsController()
{
	timer.Start();
	FireTimer.Start();

	currRotation = RotationEnum::UP;

	
	sound.Initialize("Data/Sounds/sound012.wav");

	gravity = -0.3f;
}

void FpsController::MouseMovement(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera)
{

	const float cameraSpeed = 0.015f;
	const float cameraRotSpeed = 0.003f;
	//OutputDebugStringA((std::to_string(FireTimer.GetMilisecondsElapsed()) + "\n").c_str());


	XMFLOAT4 rightFloat4;
	XMStoreFloat4(&rightFloat4, camera.GetRightVector());
	XMFLOAT4 forwardFloat4;
	XMStoreFloat4(&forwardFloat4, camera.GetForwardVector());


	while (!mouse.EventBufferIsEmpty())
	{

		if (camera.PossessCharacter)
		{
			MouseEvent me = mouse.ReadEvent();

			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				
				camera.AdjustRotation(static_cast<float>(me.GetPosY()) * cameraRotSpeed, static_cast<float>(me.GetPosX()) * cameraRotSpeed, 0.0f, true);
				camera.AdjustPosition(rightFloat4.x * -cameraSpeed  * static_cast<float>(me.GetPosX()), forwardFloat4.y * -cameraSpeed  * static_cast<float>(me.GetPosY()), rightFloat4.z * -cameraSpeed  * static_cast<float>(me.GetPosX()));
			}
		

		}

	}

}

void FpsController::Movement(float& dt, GameObject& gameObject, std::vector<GameObject*>& attachedGameObjects, physx::PxControllerManager& manager, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera)
{
	//MouseEvent me = mouse.ReadEvent();
	//if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
	//{
	//	OutputDebugStringA("MOVE!!!\n");
	//}


	//if(camera.rot.x>0)
	//	gameObject.rot.x = camera.rot.x*1.2f;
	//else if(camera.rot.x<0)
	//	gameObject.rot.x = camera.rot.x *0.9f;
	//else
	//	gameObject.rot.x = camera.rot.x;


	//OutputDebugStringA(("YAW = " + std::to_string(camera.pitch) + "\n").c_str());

	for (int i = 0; i < manager.getNbControllers(); ++i)
	{
		if (manager.getController(i)->getActor()->getName() == gameObject.objectName)
		{
			gameObject.isMovingLeft = false;
			
				XMFLOAT4 forwardDir;
				XMStoreFloat4(&forwardDir, camera.vec_forward);
				XMFLOAT4 rightDir;
				XMStoreFloat4(&rightDir, camera.vec_right);


				float velocity = 0.0005 * dt;
				float moveX = 0.0f;
				float moveZ = 0.0f;


				gameObject.phsxObj.trans = manager.getController(i)->getActor()->getGlobalPose();

				gameObject.phsxObj.trans.q = physx::PxQuat((camera.rot.y), physx::PxVec3(0, 1, 0));
				
				manager.getController(i)->getActor()->setGlobalPose(gameObject.phsxObj.trans);

				gameObject.matrix_rotate = XMMatrixRotationRollPitchYaw(gameObject.rot.x, gameObject.rot.y, gameObject.rot.z);

				gameObject.matrix_rotate *= XMMatrixRotationAxis(XMVECTOR{ 0,1, 0 }, camera.rot.y);

				if (keyboard.KeyIsPressed(VK_F8))
				{
					camera.PossessCharacter = true;
				}
				if (keyboard.KeyIsPressed(VK_F9))
				{
					camera.PossessCharacter = false;
				}

				//TODO: FIX CAMERA OFFSET FROM CHARACTER MODEL
				//OutputDebugStringA(("X = " + std::to_string(forwardDir.x) + "\t" + "|Z = " + std::to_string(forwardDir.z) + "\n").c_str());

				gameObject.pos = XMFLOAT3((float)manager.getController(i)->getPosition().x, (float)manager.getController(i)->getPosition().y, (float)manager.getController(i)->getPosition().z);
				if (camera.PossessCharacter)
				{
					
					camera.SetPosition(XMVECTOR{ gameObject.pos.x,gameObject.pos.y + 0.7f ,gameObject.pos.z});



					if (keyboard.KeyIsPressed('S') && (keyboard.KeyIsPressed('A')))
					{
						currRotation = RotationEnum::LEFT_DOWN;
						gameObject.isMovingRight = true;
						gameObject.isMovingLeft = true;
					
						moveX = -velocity * forwardDir.x - velocity * rightDir.x;
						moveZ = -velocity * forwardDir.z - velocity * rightDir.z;

						gameObject.model.currAnim = 1;
					}
					else if (keyboard.KeyIsPressed('S') && (keyboard.KeyIsPressed('D')))
					{
						currRotation = RotationEnum::RIGHT_DOWN;
						gameObject.isMovingRight = true;

						moveX = -velocity * forwardDir.x + velocity * rightDir.x;
						moveZ = -velocity * forwardDir.z + velocity * rightDir.z;
			
						gameObject.model.currAnim = 1;
					}
					else if (keyboard.KeyIsPressed('W') && (keyboard.KeyIsPressed('A')))
					{
						currRotation = RotationEnum::LEFT_UP;
						gameObject.isMovingRight = true;
						gameObject.isMovingLeft = true;
						moveX = velocity * forwardDir.x - velocity * rightDir.x;
						moveZ = velocity * forwardDir.z - velocity * rightDir.z;
						
					
						gameObject.model.currAnim = 1;
					}
					else if (keyboard.KeyIsPressed('W') && (keyboard.KeyIsPressed('D')))
					{
						currRotation = RotationEnum::RIGHT_UP;
						gameObject.isMovingRight = true;
			
						moveX = velocity * forwardDir.x + velocity * rightDir.x;
						moveZ = velocity * forwardDir.z + velocity * rightDir.z;

						gameObject.model.currAnim = 1;
					}
					else
					{
						gameObject.isMovingRight = false;
						if (keyboard.KeyIsPressed('W'))
						{
							currRotation = RotationEnum::UP;
							gameObject.isMovingFront = true;
			
							moveX = velocity * forwardDir.x;
							moveZ = velocity * forwardDir.z;
	
						}
						else if (keyboard.KeyIsPressed('S'))
						{
							currRotation = RotationEnum::DOWN;
							gameObject.isMovingFront = true;

							moveX = -velocity * forwardDir.x;
							moveZ = -velocity * forwardDir.z;
					

							gameObject.model.currAnim = 1;

						}
						else
						{
							gameObject.isMovingFront = false;
						}


						if (keyboard.KeyIsPressed('D'))
						{
						
							currRotation = RotationEnum::RIGHT;
							gameObject.isMovingRight = true;
			
							moveX = velocity * rightDir.x;
							moveZ = velocity * rightDir.z;
	


						}
						else if (keyboard.KeyIsPressed('A'))
						{
							currRotation = RotationEnum::LEFT;
							gameObject.isMovingRight = true;
							gameObject.isMovingLeft = true;
		

							moveX = -velocity * rightDir.x;
							moveZ = -velocity * rightDir.z;
			

							gameObject.model.currAnim = 1;

						}
						else
						{
							gameObject.isMovingRight = false;
						}


						if ((!gameObject.isMovingFront && !gameObject.isMovingRight) || gameObject.isFalling)
						{
							gameObject.model.currAnim = 0;

							for (int i = 0; i < attachedGameObjects.size(); ++i)
							{
								if (attachedGameObjects[i])
								{
									attachedGameObjects[i]->model.SetAnimIndex(0, true, 0.4);
									attachedGameObjects[i]->model.Update();
								}
									
							}
							

						}
						else if(gameObject.isMovingFront || gameObject.isMovingRight)
						{
							for (int i = 0; i < attachedGameObjects.size(); ++i)
							{
								if (attachedGameObjects[i])
								{
									attachedGameObjects[i]->model.SetAnimIndex(1, true, 0.6);
									attachedGameObjects[i]->model.Update();
								}
								
							}
							
						}
					}
				}
				for (int i = 0; i < attachedGameObjects.size(); ++i)
				{
					if (attachedGameObjects[i])
					{
						attachedGameObjects[i]->model.Update();
					}
				}
				manager.getController(i)->move(physx::PxVec3(moveX, gravity, moveZ), 0.001f, dt, NULL);

		}
	}



}



void FpsController::Actions(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera)
{
	if (camera.PossessCharacter)
	{
		val += (0.0001f * dt);
		//OutputDebugStringA(("VAL = " + std::to_string(val) + "\n").c_str());
		FireTimer.Restart();
		if (val > 1.0f)
		{
			canFire = true;
	
		}
		else
		{
	
			canFire = false;
		}
	
		Fire(mouse, gameObject);
	
	
		XMFLOAT4 forwardDir;
		XMStoreFloat4(&forwardDir, camera.vec_forward);
		XMFLOAT4 rightDir;
		XMStoreFloat4(&rightDir, camera.vec_right);
		//if (canSlide && !gameObject.isFalling)
		//{
		//	if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('A'))
		//	{
		//
		//		gameObject.body->applyCentralImpulse(btVector3(-rightDir.x * 1000*dt, 0, -rightDir.z * 1000*dt));
		//
		//		if (timer.GetMilisecondsElapsed() > 500.0f)
		//		{
		//			timer.Restart();
		//			canSlide = false;
		//		}
		//
		//	}
		//	else if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('D'))
		//	{
		//		gameObject.body->applyCentralImpulse(btVector3(rightDir.x * 1000, 0, rightDir.z * 1000));
		//
		//		if (timer.GetMilisecondsElapsed() > 500.0f)
		//		{
		//			timer.Restart();
		//			canSlide = false;
		//		}
		//
		//	}
		//	else if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('W'))
		//	{
		//		gameObject.body->applyCentralImpulse(btVector3(forwardDir.x * 1000, 0, forwardDir.z * 1000));
		//
		//		if (timer.GetMilisecondsElapsed() > 500.0f)
		//		{
		//			timer.Restart();
		//			canSlide = false;
		//		}
		//
		//	}
		//	else if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('S'))
		//	{
		//		gameObject.body->applyCentralImpulse(btVector3(-forwardDir.x * 1000, 0, -forwardDir.z * 1000));
		//
		//		if (timer.GetMilisecondsElapsed() > 300.0f)
		//		{
		//			timer.Restart();
		//			canSlide = false;
		//		}
		//	}
		//	else
		//	{
		//		canSlide = false;
		//	}
		//
		//
		//
		//
		//}
		//
		//if (!canSlide)
		//{
		//	if (timer.GetMilisecondsElapsed() > 200.0f)
		//	{
		//		timer.Restart();
		//		canSlide = true;
		//	}
		//}
		//
		//
		//
		//
		//if (keyboard.KeyIsPressed(VK_SPACE))
		//{
		//	if (!gameObject.isFalling)
		//	{
		//			if (gameObject.isMovingFront || gameObject.isMovingRight || gameObject.isMovingLeft)
		//			{
		//				gameObject.body->m_linearVelocity.setY(7);
		//				//gameObject.body->setLinearVelocity(btVector3(gameObject.body->getLinearVelocity().getX(), 7, gameObject.body->getLinearVelocity().getZ()));
		//			}
		//			else
		//			{
		//				gameObject.body->m_linearVelocity.setY(6);
		//				//gameObject.body->setLinearVelocity(btVector3(gameObject.body->getLinearVelocity().getX(), 6, gameObject.body->getLinearVelocity().getZ()));
		//			}
		//	}
		//
		//}
		
	}



}

void FpsController::Fire(MouseClass& mouse,GameObject& gameObject)
{

	if (canFire)
	{
		
		if (mouse.IsLeftDown())
		{
			if (!sound.Play())
			{
				return;
			}
			val = 0.0;
			//FireTimer.Restart();

			isFiring = true;
			
				
		}
	}
	else
	{
		isFiring = false;
	}
	
	gameObject.canFire = canFire;
	gameObject.isFiring = isFiring;
	//OutputDebugStringA((std::to_string(FireTimer.GetMilisecondsElapsed()) + "\n").c_str());
	
}

void FpsController::UpdateSounds()
{
	sound.Update();
}
