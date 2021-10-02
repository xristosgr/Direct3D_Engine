#pragma once
#include "WindowContainer.h"
#include "AppTimer.h"
#include "PlayerCharacter.h"
#include "FpsController.h"
#include "Game.h"
#include "NavMeshClass.h"

class Engine : virtual WindowContainer, public Game
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update(int width, int height);
	void RenderFrame(float& dt);


private:

	void TestIntersection(int mouseX, int mouseY, int screenWidth, int screenHeight);
	bool RaySphereIntersect(DirectX::XMVECTOR& rayOrigin, DirectX::XMVECTOR& rayDirection, float radius);
	bool InitializeSound(SoundSystem* sound,const char* filePath);

	int selectedObject = NULL;

	bool rightDown = false;

	
protected:
	AppTimer timer;
	AppTimer physicsTimer;

public:
	std::vector<GameObject*> gameObjects;
	std::vector<PointLight> pointLights;



	unsigned int lastMousePressed = 0;
	int listbox_item_current = -1;
	std::vector< const char*> objNames;

	Camera camera;

	bool showCursor = true;
	bool runPhysicsSimulation = false;


	double val = 0.0;
	bool HideUI = false;

private:
	PhysXHandler physXHandler;
	PhysXDebugDraw physXDebugDraw;

	bool isNavMeshCreated = false;
	//std::future<void> m_async;
	std::future<void> simple_grid_async;
	//std::vector<std::future<void>> async_test;
	std::vector<std::thread > thread_test;
	std::thread grid_test;
};