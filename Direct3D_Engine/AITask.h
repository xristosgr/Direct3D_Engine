#pragma once
#include "GameObject.h"

class AITask
{
public:
	AITask();

	virtual void Enter(GameObject* gameObject) = 0;
	virtual void Execute(GameObject* gameObject) = 0;
	virtual void Exit(GameObject* gameObject) = 0;
};

