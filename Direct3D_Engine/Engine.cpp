#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{

	timer.Start();
	physicsTimer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(),camera, width, height))
		return false;

	
	gfx.InitializeObjects(gameObjects, pointLights,camera, objNames);


	//grid.Initialize(gfx.device, gfx.deviceContext, DirectX::XMMatrixIdentity(), gfx.cb_vs_vertexshader);

	navMeshes.resize(gameObjects.size());

	runPhysicsSimulation = false;
	physXHandler.Initialize();
	physXHandler.InitializeObjects(gameObjects);


	for (int i = 0; i < gameObjects.size(); ++i)
	{
		gameObjects[i]->model.importers.clear();
		gameObjects[i]->model.vertices.clear();
		if (gameObjects[i]->model.indices.size() > 0)
			gameObjects[i]->model.indices.clear();


	}

	thread_test.resize(navMeshes.size());
	return true;
}

bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

void Engine::Update(int width,int height)
{

	ImGuiIO& io = ImGui::GetIO();
	float delta = 1000.0f / io.Framerate;

	//float dt = timer.GetMilisecondsElapsed();
	//val += (0.000001f * dt);
	//OutputDebugStringA(("VAL = " + std::to_string(val) + "\n").c_str());
	float t = timer.GetMilliseconds();
	timer.Restart();

	int physicsTimerDt = physicsTimer.GetSecondsElapsed();
	physicsTimer.Restart();

	//OutputDebugStringA((std::to_string(physicsTimerDt) + "\n").c_str());

	while (!keyboard.CharBufferIsEmpty())
	{
		unsigned char ch = keyboard.ReadChar();
	}

	while (!keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}


	if (!camera.PossessCharacter)
	{
		float cameraSpeed = 0.005f;

		
		while (!mouse.EventBufferIsEmpty())
		{
			MouseEvent me = mouse.ReadEvent();
			
			if (mouse.IsMiddleDown())
			{
				if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
				{
					camera.AdjustRotation(static_cast<float>(me.GetPosY()) * 0.01f, static_cast<float>(me.GetPosX()) * 0.01f, 0.0f, true);

				}
				
			}
			if (mouse.IsRightDown())
			{
				
				lastMousePressed = 1;

				rightDown = true;
			}
			else
			{
				rightDown = false;
			}
			if (mouse.IsLeftDown())
			{
				lastMousePressed = 2;
			}

		}
		//OutputDebugStringA(("MousePressed = " + std::to_string(gfx.lastMousePressed) + "\n").c_str());
		

		if (keyboard.KeyIsPressed(VK_SHIFT))
		{
			cameraSpeed = 0.1;
		}

		if (keyboard.KeyIsPressed('W'))
		{
			camera.AdjustPosition(camera.GetForwardVector() * cameraSpeed * delta);
		}
		if (keyboard.KeyIsPressed('S'))
		{
			camera.AdjustPosition(camera.GetBackwardVector() * cameraSpeed * delta);
		}

		if (keyboard.KeyIsPressed('A'))
		{
			camera.AdjustPosition(camera.GetLeftVector() * cameraSpeed * delta);

		}
		if (keyboard.KeyIsPressed('D'))
		{
			camera.AdjustPosition(camera.GetRightVector() * cameraSpeed * delta);
		}


		if (keyboard.KeyIsPressed(VK_SPACE))
		{
			camera.AdjustPosition(0.0f, cameraSpeed * delta, 0.0f);
		}
		if (keyboard.KeyIsPressed('Q'))
		{
			camera.AdjustPosition(0.0f, -cameraSpeed * delta, 0.0f);
		}

		if (keyboard.KeyIsPressed(VK_RETURN))
		{
			listbox_item_current = -1;
			selectedObject = NULL;
		}
		
	
	}
	//OutputDebugStringA(("MouseX = " + std::to_string(mouse.GetPosX()) + "MouseY = " + std::to_string(mouse.GetPosY()) + "\n").c_str());


	if (keyboard.KeyIsPressed('1'))
	{
		this->gfx.SetDebugWindowMode(0);
	}
	if (keyboard.KeyIsPressed('2'))
	{
		this->gfx.SetDebugWindowMode(1);
	}
	if (keyboard.KeyIsPressed('3'))
	{
		this->gfx.SetDebugWindowMode(2);
	}
	if (keyboard.KeyIsPressed('4'))
	{
		this->gfx.SetDebugWindowMode(3);
	}
	if (keyboard.KeyIsPressed('5'))
	{
		this->gfx.SetDebugWindowMode(4);
	}
	if (keyboard.KeyIsPressed('6'))
	{
		this->gfx.SetDebugWindowMode(5);
	}

	if (keyboard.KeyIsPressed(VK_F2))
	{
		camera.PossessCharacter = false;
	}
	if (keyboard.KeyIsPressed(VK_F3))
	{
		camera.PossessCharacter = true;
	}
	if (keyboard.KeyIsPressed(VK_F8))
	{
		camera.PossessCharacter = true;
		runPhysicsSimulation = true;
		RECT rect;
		GetClientRect(this->render_window.GetHWND(), &rect);
		MapWindowPoints(this->render_window.GetHWND(), nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}
	if (keyboard.KeyIsPressed(VK_F9))
	{
		camera.PossessCharacter = false;
		runPhysicsSimulation = false;
		ClipCursor(nullptr);
	}
	//if (keyboard.KeyIsPressed(VK_F1))
	//{
	//	runPhysicsSimulation = true;
	//}
	//if (keyboard.KeyIsPressed(VK_F2))
	//{
	//	runPhysicsSimulation = false;
	//}
	if (keyboard.KeyIsPressed(VK_ESCAPE))
	{
		this->render_window.~RenderWindow();
	}
	if (keyboard.KeyIsPressed(VK_F6))
	{
		HideUI = true;
	}
	if (keyboard.KeyIsPressed(VK_F7))
	{
		HideUI = false;
	}
	if (keyboard.KeyIsPressed(VK_INSERT))
	{
		//camera.SetPosition(gameObjects[selectedObject]->pos);
		if (listbox_item_current != -1 && !gameObjects[listbox_item_current]->model.deleted)
		{
			if (gameObjects[listbox_item_current]->isAttachedToObj)
				camera.SetPosition(gameObjects[listbox_item_current]->parentObj->pos);
			else
				camera.SetPosition(gameObjects[listbox_item_current]->pos);
		}
			
	}
	


	//if (runPhysicsSimulation)
	//{
	//	RECT rect;
	//	GetClientRect(this->render_window.GetHWND(),&rect);
	//	MapWindowPoints(this->render_window.GetHWND(), nullptr, reinterpret_cast<POINT*>(&rect), 2);
	//	ClipCursor(&rect);
	//}
	//else
	//{
	//	ClipCursor(nullptr);
	//
	//}
	RenderFrame(delta);
}

void Engine::RenderFrame(float& dt)
{
	
	bool isInEditMode = false;
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (gameObjects[i]->isInEditMode)
		{
			isInEditMode = true;
			break;
		}
	}
	float pointX = (float)mouse.GetPosX();
	float pointY = (float)mouse.GetPosY();

	physXHandler.InitializeObjects(gameObjects);

	if(!isInEditMode)
		RunGame(dt, gameObjects,*physXHandler.manager, keyboard, mouse, camera,runPhysicsSimulation);


	physXHandler.CharacterCollisonDetection(camera, *player, gameObjects);
	
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (runPhysicsSimulation)
		{
			if (gameObjects[i]->phsxObj.aActor)
			{
				gameObjects[i]->phsxObj.aActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
			}
			else if (gameObjects[i]->phsxObj.aStaticActor)
			{
				gameObjects[i]->phsxObj.aStaticActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);
			}
		}
		else
		{
			if (gameObjects[i]->phsxObj.aActor)
			{
				gameObjects[i]->phsxObj.aActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
			}
			else if (gameObjects[i]->phsxObj.aStaticActor)
			{
				gameObjects[i]->phsxObj.aStaticActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
			}
		}
	}
	

	physXHandler.advance(dt, gfx.vSync);
	physXHandler.aScene->fetchResults(true);
	


	if (!isInEditMode)
	{
		physXHandler.MouseRayCast(gameObjects, camera, mouse, keyboard, window_width, window_height, selectedObject, objNames, listbox_item_current);
		physXHandler.FallCheck(gameObjects);
	}
	else
	{
		runPhysicsSimulation = false;
	}

	if (grid.bCreateMesh)
	{

		//physXHandler.thread_createNavMesh.resize(grid.nodes.size());

		isNavMeshCreated = false;
		grid.isReady = false;
		grid.nodes.clear();
		navMeshes.clear();
		navMeshes.resize(gameObjects.size());
		grid.Initialize(gfx.device, gfx.deviceContext, DirectX::XMMatrixIdentity(), gfx.cb_vs_vertexshader);
		grid.bCreateMesh = false;

		
	}
	if (!isNavMeshCreated && grid.isReady)
	{
		grid_test = std::thread(&PhysXHandler::NavMeshRayCast, &physXHandler, std::ref(grid), std::ref(gameObjects));

		if (grid_test.joinable())
		{
			grid_test.join();
		}
		//physXHandler.NavMeshRayCast(grid, gameObjects);
		for (int i = 0; i < navMeshes.size(); ++i)
		{
			if (gameObjects[i]->isAI && !gameObjects[i]->model.deleted)
				thread_test[i] = std::thread(&GridClass::CreatePathGrid, &grid, std::ref(navMeshes[i].validNodes));
		}
	
		isNavMeshCreated = true;
	}

	for (int i = 0; i < thread_test.size(); ++i)
	{
		if (thread_test[i].joinable())
		{
			thread_test[i].join();
		}
	}
	
	
	physXHandler.UpdatePhysX(dt, gameObjects, mouse, keyboard,runPhysicsSimulation);

	

	
	this->gfx.RenderFrame(dt, gameObjects, pointLights,camera, physXHandler, physXDebugDraw, HideUI, listbox_item_current, objNames, keyboard, mouse, pointX, pointY, window_width, window_height, rightDown, selectedObject, grid, navMeshes[14]);

	for (int i = 0; i < pointLights.size(); ++i)
	{
		if (pointLights[i].isAttached)
		{
			DirectX::XMFLOAT3 forwardTemp;
			DirectX::XMStoreFloat3(&forwardTemp, camera.GetForwardVector());
		
			if(player)
				pointLights[i].pos = DirectX::XMFLOAT3(player->pos.x - forwardTemp.x*4, player->pos.y+ forwardTemp.y, player->pos.z - forwardTemp.z*4);
			pointLights[i].direction = XMFLOAT3(forwardTemp.x + pointLights[i].offset.x , forwardTemp.y + pointLights[i].offset.y, forwardTemp.z + pointLights[i].offset.z);
		}
	}
}

void Engine::TestIntersection(int mouseX, int mouseY,int screenWidth,int screenHeight)
{
	float pointX, pointY;
	DirectX::XMMATRIX projectionMatrix, viewMatrix, inverseViewMatrix, worldMatrix, translateMatrix, inverseWorldMatrix;
	DirectX::XMFLOAT3 direction;
	DirectX::XMVECTOR origin, rayOrigin, rayDirection;
	bool intersect, result;

	pointX = ((2.0f * (float)mouseX) / (float)screenWidth) - 1.0f;
	pointY = (((2.0f * (float)mouseY) / (float)screenHeight) -1.0f)* -1.0f;

	DirectX::XMFLOAT4X4 projection;
	DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
	pointX = pointX / projection._11;
	pointY = pointY / projection._22;

	viewMatrix = camera.GetViewMatrix();

	inverseViewMatrix = DirectX::XMMatrixInverse(nullptr, viewMatrix);

	DirectX::XMFLOAT4X4 invView;
	DirectX::XMStoreFloat4x4(&invView, inverseViewMatrix);

	direction.x = (pointX * invView._11) + (pointY * invView._21) + invView._31;
	direction.y = (pointX * invView._12) + (pointY * invView._22) + invView._32;
	direction.z = (pointX * invView._13) + (pointY * invView._23) + invView._33;

	origin = XMLoadFloat3(&camera.pos);

	XMVECTOR dirVec = XMLoadFloat3(&direction);
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		worldMatrix = gameObjects[i]->worldMatrix;
		inverseWorldMatrix = DirectX::XMMatrixInverse(nullptr, worldMatrix);
		rayOrigin = DirectX::XMVector3TransformCoord(origin, inverseWorldMatrix);
		rayDirection = DirectX::XMVector3TransformNormal(dirVec, inverseWorldMatrix);
		rayDirection = DirectX::XMVector3Normalize(rayDirection);

		intersect = RaySphereIntersect(rayOrigin, rayDirection, 1.0f);

		if (intersect == true)
		{
			//OutputDebugStringA("YES!\n");
			//this->gfx.gameObjects[i]->pos.x = direction.x*4;
			//this->gfx.gameObjects[i]->pos.y = direction.y*4;
			//this->gfx.gameObjects[i]->pos.z = direction.z*4;
		}
		else
		{
			//OutputDebugStringA("NO!\n");
		}
		//OutputDebugStringA("NO!\n");
	}


}

bool Engine::RaySphereIntersect(DirectX::XMVECTOR& rayOrigin, DirectX::XMVECTOR& rayDirection, float radius)
{
	float a, b, c, discriminant;

	XMFLOAT3 dir;
	DirectX::XMStoreFloat3(&dir, rayDirection);
	XMFLOAT3 origin;
	DirectX::XMStoreFloat3(&origin, rayOrigin);

	a = (dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z);
	b = ((dir.x * origin.x) + (dir.y * origin.y) + (dir.z * origin.z)) * 2.0f;
	c = ((origin.x * origin.x) + (origin.y * origin.y) + (origin.z * origin.z)) - (radius * radius);
	discriminant = (b * b) - (4 * a * c);
	if (discriminant < 0.0f)
	{
		return false;
	}


	return true;
}

bool Engine::InitializeSound(SoundSystem* sound, const char* filePath)
{
	if (!sound->Initialize("Data/Sounds/sound012.wav"))
	{
		return false;
	}

	return true;
}



