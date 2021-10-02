#pragma once
#include "GameObject.h"
#include "AppTimer.h"

#include "EngineVariables.h"


class CharacterController
{
public:
	CharacterController() {};

	virtual void MouseMovement(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) = 0;
	virtual void Movement(float& dt, GameObject& gameObject, std::vector<GameObject*>& attachedGameObjects, physx::PxControllerManager& manager, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) = 0;
	virtual void Actions(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) = 0;
		
};

