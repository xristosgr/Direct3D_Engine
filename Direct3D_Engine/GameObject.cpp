#include "GameObject.h"
#include <fstream>


GameObject::GameObject()
{
	locToMove = physx::PxVec3(0, 0, 0);
	isWalkable = true;
	isObstacle = false;
	isAttachedToObj = false;
	//model.headRot = XMFLOAT3(0, 0, 0);
	physxScale = physx::PxVec3(1, 1, 1);
	radius = 1.0f;

	scale = DirectX::XMFLOAT3(1, 1, 1);
	rot = DirectX::XMFLOAT3(0, 0, 0);
	pos = DirectX::XMFLOAT3(0, 0, 0);
	frustumScale = DirectX::XMFLOAT3(1, 1, 1);

	//collisionPos = btVector3(pos.x,pos.y,pos.z);

	camPos.y = -pos.y - 9.0f;

	constructShape = true;
	objectSelected = false;
	_listbox_item_current = -1;
	phsxObj.physXShapeType = PhysXObject::PhysXShapeType::eINVALID;
	physXNames.push_back("eSPHERE");
	physXNames.push_back("ePLANE");
	physXNames.push_back("eCAPSULE");
	physXNames.push_back("eBOX");
	physXNames.push_back("eCONVEXMESH");
	physXNames.push_back("eTRIANGLEMESH");
	physXNames.push_back("eDOOR");
}


bool GameObject::Initialize(const std::string filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool isAnimated, bool isPBR, bool isTextured, bool isEmptyTarget, bool isObjectDeleted)
{
	AnimCount = 0;
	this->filePath = filePath;
	this->isAnimated = isAnimated;
	this->isPBR = isPBR;
	this->isTextured = isTextured;
	this->emptyTarget = isEmptyTarget;
	this->model.deleted = isObjectDeleted;

	if (!emptyTarget)
	{
		if (!this->model.Initialize(filePath, device, deviceContex, cb_vs_vertexshader, this->isAnimated, this->isPBR, this->isTextured))
		{
			OutputDebugStringA("FAILED TO CREATE MODEL!!!!\n");
			return false;
		}
	}

	//tempModelTranslation = translateModel;
	return true;
}

void GameObject::DebugDraw(const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured)
{
	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX translate = XMMatrixTranslation(pos.x, pos.y, pos.z);


	worldMatrix = rotate * scale * translate;

	if (!emptyTarget)
		model.Draw(worldMatrix, viewMatrix, projectionMatrix, isTextured);

}
void GameObject::Draw(const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, bool isTextured,bool frustumCheck)
{
	//if(body)
	//	OutputDebugStringA((objectName+".ACTIVATION_STATE : " + std::to_string(body->getActivationState()) + "\n").c_str());
		



	matrix_scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);

	if (!phsxObj.isCharacter)
	{
		matrix_rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		matrix_translate = XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

	
	if (!phsxObj.isCharacter)
	{
		if (!UpdatePhysX(matrix_scale, matrix_rotate, matrix_translate))
		{

			matrix_rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
			matrix_translate = XMMatrixTranslation(pos.x + translateModel.x, pos.y + translateModel.y, pos.z+translateModel.z);
		}

	}
	

	
	worldMatrix = (matrix_scale * matrix_rotate) * matrix_translate;

	if (!emptyTarget)
	{
		if (frustumCheck)
		{
			if (!isAttachedToObj)
				model.Draw(worldMatrix, viewMatrix, projectionMatrix, isTextured,isPlayer);
			else
			{
				if (parentObj)
				{
					worldMatrix = (matrix_scale * matrix_rotate) *  matrix_translate;
					XMMATRIX mat = parentObj->model.worldMatrix;
					model.Draw(worldMatrix, viewMatrix, projectionMatrix, isTextured, mat, parentObj->model.FinalMatrixTrans);
				}
					
			}

		}
	}
		
			
}

void GameObject::DrawGui(std::string name, ID3D11DeviceContext* deviceContex, KeyboardClass& keyboard, Camera& camera)
{

	if (!isInEditMode)
	{
		if (ImGui::Button("EditMode"))
		{
			isInEditMode = true;
			XMFLOAT3 _pos = XMFLOAT3(pos.x , pos.y, pos.z+5.0f);
			camera.SetPosition(_pos);
		
			
		}
		if (keyboard.KeyIsPressed('G'))
		{
			mouseFollow = true;
		}
		else
		{
			mouseFollow = false;
		}
		if (objectName == "" && !nameInitialized)
		{
			objectName = name;
			nameInitialized = true;
		}

		windowActive = true;
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::Text(objectName.c_str());
		ImGui::InputText("Rename", (char*)tempName.c_str(), 100);
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			objectName = tempName.data();
		}
		ImGui::Text(("X: " + std::to_string(pos.x) + " |Y: " + std::to_string(pos.y) + " |Z: " + std::to_string(pos.z)).c_str());
		if (phsxObj.aActor || phsxObj.aStaticActor || phsxObj.isCharacter)
		{
			ImGui::DragFloat3("TranslatePhysXOBJ", &phsxObj.updatePos.x, 0.02f);
			ImGui::DragFloat3("PhysxScale", &physxScale.x, 0.005f);
			ImGui::DragFloat4("RotatePhysXOBJ", &phsxObj.updateRot.x, 0.01f);
			ImGui::DragFloat3("Scale", &scale.x, 0.005f);
			ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
			ImGui::DragFloat3("translateModel", &translateModel.x, 0.005f);
			if (phsxObj.jointActor)
			{
				ImGui::DragFloat3("JoinPos", &phsxObj.jointPos.x, 0.005f);
			}
			if (ImGui::Button("createDoor"))
			{
				phsxObj.bCreateDoor = true;
			}

		}
		else
		{
			if(isAttachedToObj)
				ImGui::DragFloat3("Translation", &pos.x, 0.005f);
			else
				ImGui::DragFloat3("Translation", &pos.x, 0.005f);
			ImGui::DragFloat3("Scale", &scale.x, 0.005f);
			ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
			ImGui::DragFloat3("translateModel", &translateModel.x, 0.005f);
		}

		if (ImGui::Button("delete") || keyboard.KeyIsPressed(VK_DELETE))
		{
			for (int i = 0; i < model.scenes.size(); ++i)
			{
				for (int j = 0; j < model.scenes[i]->mNumMeshes; ++j)
				{
					model.scenes[i]->mMeshes[j]->~aiMesh();
				}
				//for (int j = 0; j < model.scenes[i]->mNumMaterials; ++j)
				//{
				//	model.scenes[i]->mMaterials[j]->~aiMaterial();
				//}
				//for (int j = 0; j < model.scenes[i]->mNumTextures; ++j)
				//{
				//	model.scenes[i]->mTextures[j]->~aiTexture();
				//}
				for (int j = 0; j < model.scenes[i]->mNumAnimations; ++j)
				{
					model.scenes[i]->mAnimations[j]->~aiAnimation();
				}
				//for (int j = 0; j < model.scenes[i]->mNumCameras; ++j)
				//{
				//	model.scenes[i]->mCameras[j]->~aiCamera();
				//}
			}
			for (int i = 0; i < model.importers.size(); ++i)
			{
				//model.importers[i]->FreeScene();
				model.importers[i]->~Importer();
			}

			model.scenes.clear();
			objectName = "<empty>";
			if (model._textures.size() > 0)
			{
				for (int i = 0; i < model._textures.size(); ++i)
				{
					model._textures[i].textureView.Get()->Release();
					model._textures[i].texture.Get()->Release();
				}
				model._textures.clear();
			}
			model.deleted = true;
			for (int i = 0; i < model.meshes.size(); ++i)
			{
				if (model.isAnimated)
					model.meshes[i].boneDataBuffer.Get()->Release();
				isAnimated = false;
				AnimCount = 0;
				model.meshes[i].deleted = true;

				model.meshes[i].vertexBuffer.Get()->Release();
			}


			isPlayer = false;
			model.m_BoneInfo.clear();
			model.BoneMapping.clear();
			//delete model.m_bone_location;
			model.node_anims.clear();
			model.importers.clear();
			model.vertices.clear();
			model.indices.clear();
			model.BoneMapping.clear();
			model.importers.clear();
			//model.scenes.clear();
			model._textures.clear();
			OutputDebugStringA("DELETED!!!!\n");

		}
		deviceContex->Flush();
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::NewLine();
	}
	else
	{
		camera.SetLookAtPos(XMFLOAT3(pos.x, pos.y+1.0f, pos.z));
		if (ImGui::Button("Exit EditMode"))
		{
			isInEditMode = false;
		}
		if (objectName == "" && !nameInitialized)
		{
			objectName = name;

			nameInitialized = true;

		}


		windowActive = true;
		ImGui::NewLine();
		ImGui::NewLine();
		ImGui::Text(objectName.c_str());
		ImGui::InputText("Rename", (char*)tempName.c_str(), 100);
		ImGui::SameLine();
		if (ImGui::Button("Apply"))
		{
			objectName = tempName.data();
		}

		if (ImGui::CollapsingHeader("more"))
		{
			ImGui::Checkbox("isWalkable", &isWalkable);
			ImGui::Checkbox("isObstacle", &isObstacle);
			ImGui::Checkbox("AttachToObject", &isAttachedToObj);
			ImGui::Checkbox("Transparent", &isTransparent);
			ImGui::Checkbox("Player", &isPlayer);
			ImGui::Checkbox("AI", &isAI);
			ImGui::Checkbox("frustum", &frustumEnable);
			ImGui::Checkbox("attachToCamera", &attachToCamera);
		}

		if (ImGui::CollapsingHeader("Physics Params"))
		{
			ImGui::Text(("IsFalling = " + std::to_string(isFalling)).c_str());

			ImGui::Checkbox("IsCharacter", &phsxObj.isCharacter);
			ImGui::SameLine;
			ImGui::Checkbox("IsKinematic", &phsxObj.isKinematic);
			ImGui::ListBox("PhysicsShape", &_listbox_item_current, physXNames.data(), physXNames.size(), 5);

			switch (_listbox_item_current)
			{
			case 0:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)0; break;
			case 1:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)1; break;
			case 2:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)2; break;
			case 3:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)3; break;
			case 4:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)4; break;
			case 5:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)5; break;
			case 6:
				phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)6; break;
			//case 7:
			//	phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)7; break;
			}
			ImGui::Text(("PhysXShape: " + std::to_string(phsxObj.physXShapeType)).c_str());
			if (phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eCONVEXMESH)
			{
				ImGui::InputInt("convexMeshDetail", &phsxObj.convexMeshDetail, 1);
			}
			else if (phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eTRIANGLEMESH)
			{

				ImGui::InputInt("triangleMeshStride", &phsxObj.triangleMeshStride, 1);
				ImGui::Checkbox("isDxDataType", &phsxObj.isDxDataType);
				if (ImGui::Button("UpdatedShape"))
				{
					phsxObj.bUpdateShape = true;
				}
			}

			if (ImGui::Button("CreateShape"))
			{

				constructShape = true;
			}

			ImGui::NewLine();

			ImGui::DragFloat("Mass", &phsxObj.mass, 0.005f);
			ImGui::DragFloat("Radius", &radius, 0.005f);




		
			
			

		}
		ImGui::Text(("X: " + std::to_string(pos.x) + " |Y: " + std::to_string(pos.y) + " |Z: " + std::to_string(pos.z)).c_str());

		if (phsxObj.aActor || phsxObj.aStaticActor || phsxObj.isCharacter)
		{
			ImGui::DragFloat3("TranslatePhysXOBJ", &phsxObj.updatePos.x, 0.02f);
			ImGui::DragFloat3("PhysxScale", &physxScale.x, 0.005f);
			ImGui::DragFloat4("RotatePhysXOBJ", &phsxObj.updateRot.x, 0.01f);
		}
		else
		{
			if(isAttachedToObj)
				ImGui::DragFloat3("Translation", &pos.x, 0.0001f);
			else
				ImGui::DragFloat3("Translation", &pos.x, 0.005f);
			
		}
		
		ImGui::DragFloat3("mRot", &model.mRot.x, 0.02f);
		ImGui::DragFloat3("mPos", &model.mPos.x, 0.02f);
		//ImGui::DragFloat3("headScale", &model.headScale.x, 0.02f);
		//ImGui::DragFloat3("headPos", &model.headPos.x, 0.02f);
		ImGui::DragFloat3("Scale", &scale.x, 0.005f);
		ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
		ImGui::DragFloat3("translateModel", &translateModel.x, 0.005f);
		ImGui::DragFloat3("FrustumScale", &frustumScale.x, 0.005f, 1.0f);
	}
}

void GameObject::SetScale(const XMVECTOR& scale)
{
	XMStoreFloat3(&this->scale, scale);
	this->scaleVector = scale;
}

void GameObject::SetRotation(const XMVECTOR& rot)
{
	XMStoreFloat3(&this->rot, rot);
	this->rotVector = rot;
}

void GameObject::SetTranslation(const XMVECTOR& pos)
{
	XMStoreFloat3(&this->pos, pos);
	this->posVector = pos;
}


void GameObject::SetTranslation(const XMFLOAT3& pos)
{
	this->pos = pos;
}

void GameObject::AttachToObject(GameObject& gameObject)
{
	parentObj = &gameObject;
	//pos.x = gameObject.pos.x+translateModel.x;
	//pos.y = gameObject.pos.y + translateModel.y;
	//pos.z = gameObject.pos.z + translateModel.z;

	//rot = gameObject.model.mRot;
	//scale = gameObject.scale;
}


DirectX::XMFLOAT3 GameObject::GetPositionFloat3()
{
	return pos;
}

DirectX::XMFLOAT3 GameObject::GetScaleFloat3()
{
	return scale;
}

void GameObject::SetupCamera(int windowWidth, int windowHeight)
{

	camPos = pos;
	direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	cameraReflect.SetPosition(camPos.x, -9.410, camPos.z);
	cameraReflect.SetLookAtPos(direction);
	cameraReflect.PerspectiveFov(90.0f, static_cast<float>(windowWidth / windowHeight) * 2, 1.f, 100000.0f);
	ReflectViewMatrix = cameraReflect.GetViewMatrix();


	cameraRefract.SetPosition(camPos.x, -9.410, camPos.z);
	cameraRefract.SetLookAtPos(XMFLOAT3(direction.x, -direction.y, direction.z));
	cameraRefract.PerspectiveFov(90.0f, static_cast<float>(windowWidth / windowHeight) * 2, 1.f, 100000.0f);
	cameraRefract.PerspectiveFov(90.0f, static_cast<float>(windowWidth / windowHeight) * 2, 1.f, 100000.0f);
	RefractViewMatrix = cameraRefract.GetViewMatrix();
}

Camera* GameObject::GetCamera()
{
	return &cameraReflect;
}

void GameObject::UpdateCamera()
{
	direction.y = 40.0f;

	XMFLOAT3 dir = XMFLOAT3(0, direction.y, 1.0f);
	cameraReflect.SetLookAtPos(dir);
	cameraReflect.SetPosition(pos.x, pos.y + camPos.y, pos.z);
	ReflectViewMatrix = cameraReflect.GetViewMatrix();


	XMFLOAT3 dir2 = XMFLOAT3(0, -direction.y, -1.0f);
	cameraRefract.SetLookAtPos(dir2);
	cameraRefract.SetPosition(pos.x, pos.y + camPos.y, pos.z);
	RefractViewMatrix = cameraRefract.GetViewMatrix();

}

void GameObject::AttachController(physx::PxController& characterController,bool& runPhysicsSimulation)
{

	
	phsxObj.trans = characterController.getActor()->getGlobalPose();

	if (!runPhysicsSimulation)
	{
		pos = XMFLOAT3(characterController.getPosition().x, characterController.getPosition().y, characterController.getPosition().z);
		characterController.setPosition(physx::PxExtendedVec3(phsxObj.updatePos.x, phsxObj.updatePos.y, phsxObj.updatePos.z));
		phsxObj.trans.q = physx::PxQuat(phsxObj.updateRot.w, physx::PxVec3(phsxObj.updateRot.x, phsxObj.updateRot.y, phsxObj.updateRot.z));


		phsxObj.trans.q = phsxObj.trans.q.getNormalized();


		characterController.getActor()->setGlobalPose(phsxObj.trans);




		matrix_rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		matrix_rotate *= XMMatrixRotationAxis(XMVECTOR{ phsxObj.trans.q.x,phsxObj.trans.q.y, phsxObj.trans.q.z }, phsxObj.trans.q.getAngle());

	}
	
	//pos = XMFLOAT3(characterController.getPosition().x, characterController.getPosition().y, characterController.getPosition().z);
	matrix_translate = XMMatrixTranslation(pos.x + translateModel.x, pos.y + translateModel.y, pos.z + translateModel.z);
	
	
	
}

void GameObject::Input(KeyboardClass& keyBoard, MouseClass& mouse)
{
	//if (mouseFollow)
	//{
	//	if (mouse.IsWheelUp())
	//	{
	//
	//	}
	//}
	
}

bool GameObject::UpdatePhysX(XMMATRIX& scale, XMMATRIX& rotate, XMMATRIX& translate)
{
	if (!phsxObj.isCharacter)
	{
		if (phsxObj.aActor || phsxObj.aStaticActor)
		{
			if (phsxObj.aStaticActor)
				phsxObj.trans = phsxObj.aStaticActor->getGlobalPose();
			else if (phsxObj.aActor)
			{
				if (phsxObj.isKinematic)
				{
					phsxObj.aActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
					phsxObj.aActor->setLinearVelocity(physx::PxVec3(1, -9.81f, 0));
				}
				else
					phsxObj.aActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
				phsxObj.trans = phsxObj.aActor->getGlobalPose();
			}




			phsxObj.trans.p += phsxObj.updatePos;
			phsxObj.updatePos = physx::PxVec3(0, 0, 0);

			if (phsxObj.aStaticActor)
				phsxObj.trans.q = physx::PxQuat(phsxObj.updateRot.w, physx::PxVec3(phsxObj.updateRot.x, phsxObj.updateRot.y, phsxObj.updateRot.z));


			if (phsxObj.aActor)
			{
				if (phsxObj.aActor->getActorFlags().isSet(physx::PxActorFlag::eDISABLE_SIMULATION))
				{
					phsxObj.trans.q = physx::PxQuat(phsxObj.updateRot.w, physx::PxVec3(phsxObj.updateRot.x, phsxObj.updateRot.y, phsxObj.updateRot.z));
				}
			}

			phsxObj.trans.q = phsxObj.trans.q.getNormalized();

			if (phsxObj.aActor)
			{
				phsxObj.aActor->setGlobalPose(phsxObj.trans);
				phsxObj.aActor->setMass(phsxObj.mass);
				phsxObj.aActor->setName(objectName.c_str());
			}
			else if (phsxObj.aStaticActor)
			{
				phsxObj.aStaticActor->setGlobalPose(phsxObj.trans);
				phsxObj.aStaticActor->setName(objectName.c_str());
			}

			pos = XMFLOAT3(phsxObj.trans.p.x, phsxObj.trans.p.y, phsxObj.trans.p.z);
			rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
			rotate *= XMMatrixRotationAxis(XMVECTOR{ phsxObj.trans.q.x,phsxObj.trans.q.y, phsxObj.trans.q.z }, phsxObj.trans.q.getAngle());
			translate = XMMatrixTranslation(pos.x + translateModel.x, pos.y + translateModel.y, pos.z + translateModel.z);


			if (phsxObj.jointActor)
			{
				phsxObj.jointTrans = phsxObj.jointActor->getGlobalPose();
				phsxObj.jointPos = phsxObj.jointTrans.p;
				//OutputDebugStringA((" X = " + std::to_string(phsxObj.jointPos.x)).c_str());
				//OutputDebugStringA((" Y = " + std::to_string(phsxObj.jointPos.y)).c_str());
				//OutputDebugStringA((" Z = " + std::to_string(phsxObj.jointPos.z)+"\n").c_str());
				//phsxObj.jointActor->setGlobalPose(phsxObj.jointTrans);


			}
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
	
}
