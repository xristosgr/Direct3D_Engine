#pragma once
#include"CharacterController.h"

class PlayerCharacter : public CharacterController
{
public:

	PlayerCharacter();

	void MouseMovement(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) override;
	virtual void Movement(float& dt, GameObject& gameObject, std::vector<GameObject*>& attachedGameObjects, physx::PxControllerManager& manager, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) override;
	virtual void Actions(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) override;

private:

	enum RotationEnum
	{
		UP = 0,
		RIGHT_UP = 1,
		RIGHT = 2,
		RIGHT_DOWN = 3,
		DOWN = 4,
		LEFT_DOWN = 5,
		LEFT = 6,
		LEFT_UP = 7,
	};

	void SetCharacterRotation(GameObject& gameObject, Camera& camera);
	RotationEnum currRotation;
	RotationEnum prevRotation;

	bool rotateLeft = false;
	bool rotateUp = false;
	bool rotateRight = false;

	bool canSlide = true;
	float testX = 0.0f;
	float testZ = 0.0f;

	AppTimer timer;
private:
	float CharacterRotY = 0.0f;

	bool canJump = true;
	bool isJumping = false;
	bool jumpkeyIsPressed = false;
	float gravity = -0.01f;

};

