#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter()
{
	timer.Start();
	currRotation = RotationEnum::UP;
}

void PlayerCharacter::MouseMovement(float& dt,GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera)
{
	const float cameraSpeed = 0.001f;



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
				
				if (static_cast<float>(me.GetPosY()) < 0)
				{
					if (CharacterRotY > -14.0)
						CharacterRotY -= cameraSpeed * 15;
				}
				else if (static_cast<float>(me.GetPosY()) > 0)
				{
					if (CharacterRotY < 14.0)
						CharacterRotY += cameraSpeed * 15;
				}
	
			
				
				camera.AdjustPosition(rightFloat4.x * -cameraSpeed * dt * static_cast<float>(me.GetPosX()), forwardFloat4.y * -cameraSpeed * dt * static_cast<float>(me.GetPosY()), rightFloat4.z * -cameraSpeed * dt * static_cast<float>(me.GetPosX()));

			}


		}

	}
}

void PlayerCharacter::Movement(float& dt, GameObject& gameObject, std::vector<GameObject*>& attachedGameObjects, physx::PxControllerManager& manager, KeyboardClass& keyboard, MouseClass& mouse,Camera& camera)
{
	
		XMFLOAT4 forwardDir;
		XMStoreFloat4(&forwardDir, camera.vec_forward);
		XMFLOAT4 rightDir;
		XMStoreFloat4(&rightDir, camera.vec_right);
	
	
	
		SetCharacterRotation(gameObject, camera);
	
		
		if (camera.PossessCharacter)
		{
		
			camera.SetLookAtPos(XMFLOAT3(gameObject.pos.x, gameObject.pos.y+2.2f, gameObject.pos.z));
	
			camera.SetPosition(XMVECTOR{ gameObject.pos.x + (-4.4f * std::sin(camera.yaw)),gameObject.pos.y + CharacterRotY,gameObject.pos.z + (-4.4f * std::cos(camera.yaw)) });
	
	
			for (int i = 0; i < manager.getNbControllers(); ++i)
			{
				if (manager.getController(i)->getActor()->getName() == gameObject.objectName)
				{
					gameObject.isMovingLeft = false;

					XMFLOAT4 forwardDir;
					XMStoreFloat4(&forwardDir, camera.vec_forward);
					XMFLOAT4 rightDir;
					XMStoreFloat4(&rightDir, camera.vec_right);


					float velocity = 0.008 * dt;
					//gravity = -0.015f * dt;
					float moveX = 0.0f;
					float moveZ = 0.0f;


					gameObject.phsxObj.trans = manager.getController(i)->getActor()->getGlobalPose();

					gameObject.phsxObj.trans.q = physx::PxQuat((camera.rot.y), physx::PxVec3(0, 1, 0));

					manager.getController(i)->getActor()->setGlobalPose(gameObject.phsxObj.trans);

					gameObject.matrix_rotate = XMMatrixRotationRollPitchYaw(gameObject.rot.x, gameObject.rot.y, gameObject.rot.z);

					gameObject.matrix_rotate *= XMMatrixRotationAxis(XMVECTOR{ 0,1, 0 }, camera.rot.y);

				
					//TODO: FIX CAMERA OFFSET FROM CHARACTER MODEL
					//OutputDebugStringA(("X = " + std::to_string(forwardDir.x) + "\t" + "|Z = " + std::to_string(forwardDir.z) + "\n").c_str());


					if (camera.PossessCharacter)
					{

						//camera.SetPosition(XMVECTOR{ gameObject.pos.x,gameObject.pos.y + 2.2f ,gameObject.pos.z });



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
								gameObject.model.SetAnimIndex(1, true, 0.5);

							}
							else if (gameObject.isMovingFront || gameObject.isMovingRight)
							{
								gameObject.model.SetAnimIndex(0, true, 0.7);
							}
						}
					}
					gameObject.model.Update();

					float timerCount = 20.0f;
					float delta = dt;

					if (dt < 5.0f)
					{
						delta = 5.0f;
					}
					if (dt > 12.0f)
					{
						timerCount = timerCount / 2.0f;
					}
					if (gameObject.isFalling)
					{
						keyboard.OnKeyReleased(VK_SPACE);
						if (timer.GetMilisecondsElapsed() < timerCount * delta)
						{
							gravity -= 0.0001f * delta;
						}
						else if (timer.GetMilisecondsElapsed() >= timerCount * delta && timer.GetMilisecondsElapsed() < timerCount * 2.0f * delta)
						{
							gravity -= 0.0004f * delta;
						}
						else
						{
							gravity = -0.012f * delta;


						}

					}
					else
					{
						if (keyboard.KeyIsPressed(VK_SPACE))
						{
							timer.Restart();
							gravity += 0.009f * delta;
							isJumping = true;
						}
					}

					//OutputDebugStringA(("timer = " + std::to_string(timer.GetMilisecondsElapsed()) + "\t").c_str());
					//OutputDebugStringA(("isFalling = " + std::to_string(gameObject.isFalling) + "\t").c_str());
					//OutputDebugStringA(("gravity = " + std::to_string(gravity) + "\n").c_str());

					gameObject.model.Update();
					manager.getController(i)->move(physx::PxVec3(moveX, gravity, moveZ), 0.01f, dt, NULL);
				}
			}

	
				
	
	
		}
	
	

}

void PlayerCharacter::Actions(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera)
{
	//if (camera.PossessCharacter)
	//{
	//	XMFLOAT4 forwardDir;
	//	XMStoreFloat4(&forwardDir, camera.vec_forward);
	//	XMFLOAT4 rightDir;
	//	XMStoreFloat4(&rightDir, camera.vec_right);
	//	if (canSlide)
	//	{
	//		if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('A'))
	//		{
	//
	//			gameObject.body->applyCentralImpulse(btVector3(-rightDir.x * 1000, 0, -rightDir.z * 1000));
	//
	//			//OutputDebugStringA((std::to_string(-rightDir.x * 1000) + "\n").c_str());
	//			gameObject.model.currAnim  = 2;
	//
	//			if (timer.GetMilisecondsElapsed() > 500.0f)
	//			{
	//				timer.Restart();
	//				canSlide = false;
	//			}
	//
	//		}
	//		else if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('D'))
	//		{
	//			gameObject.body->applyCentralImpulse(btVector3(rightDir.x * 1000, 0, rightDir.z * 1000));
	//			gameObject.model.currAnim  = 2;
	//
	//			if (timer.GetMilisecondsElapsed() > 500.0f)
	//			{
	//				timer.Restart();
	//				canSlide = false;
	//			}
	//
	//		}
	//		else if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('W'))
	//		{
	//			gameObject.body->applyCentralImpulse(btVector3(forwardDir.x * 1000, 0, forwardDir.z * 1000));
	//			gameObject.model.currAnim  = 2;
	//
	//			if (timer.GetMilisecondsElapsed() > 500.0f)
	//			{
	//				timer.Restart();
	//				canSlide = false;
	//			}
	//
	//		}
	//		else if (keyboard.KeyIsPressed(VK_CONTROL) && keyboard.KeyIsPressed('S'))
	//		{
	//			gameObject.body->applyCentralImpulse(btVector3(-forwardDir.x * 1000, 0, -forwardDir.z * 1000));
	//			gameObject.model.currAnim  = 2;
	//
	//			if (timer.GetMilisecondsElapsed() > 300.0f)
	//			{
	//				timer.Restart();
	//				canSlide = false;
	//			}
	//		}
	//		else
	//		{
	//			canSlide = false;
	//		}
	//
	//
	//
	//
	//	}
	//
	//	if (!canSlide)
	//	{
	//		if (timer.GetMilisecondsElapsed() > 200.0f)
	//		{
	//			timer.Restart();
	//			canSlide = true;
	//		}
	//	}
	//
	//	if (gameObject.body->getLinearVelocity().getY() >= 12.0f)
	//		gameObject.isJumping = true;
	//	if (gameObject.body->getLinearVelocity().getY() >= -0.00076 && gameObject.body->getLinearVelocity().getY() <= 0.00076)
	//	{
	//		gameObject.isJumping = false;
	//	}
	//
	//	if (!gameObject.isJumping)
	//	{
	//		if (keyboard.KeyIsPressed(VK_SPACE))
	//		{
	//			if (!gameObject.isJumping)
	//			{
	//				gameObject.body->applyCentralImpulse(btVector3(0, 50, 0));
	//
	//			}
	//
	//		}
	//	}


	//}


	
}

void PlayerCharacter::SetCharacterRotation(GameObject& gameObject,Camera& camera)
{
	//TODO: Implement third person character rotation
}
