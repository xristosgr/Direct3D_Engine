#pragma once
#include"CharacterController.h"
#include "SoundSystem.h"

class FpsController : public CharacterController
{
public:

	FpsController();

	void MouseMovement(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) override;
	virtual void Movement(float& dt, GameObject& gameObject, std::vector<GameObject*>& attachedGameObjects, physx::PxControllerManager& manager, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) override;
	virtual void Actions(float& dt, GameObject& gameObject, KeyboardClass& keyboard, MouseClass& mouse, Camera& camera) override;
	void Fire(MouseClass& mouse, GameObject& gameObject);
	void UpdateSounds();
	bool isFiring = false;
	bool canFire = true;
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

	RotationEnum currRotation;
	RotationEnum prevRotation;

	bool rotateLeft = false;
	bool rotateUp = false;
	bool rotateRight = false;

	bool canSlide = true;


	AppTimer timer;
	AppTimer FireTimer;

	SoundSystem sound;
	double val = 0.0;

	//float jumpForce = 3.0f;
	bool canJump = true;
	bool isJumping = false;
	bool jumpkeyIsPressed = false;
	float gravity = -0.1f;
};

