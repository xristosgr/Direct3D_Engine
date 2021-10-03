#include "SaveSystem.h"

SaveSystem::SaveSystem()
{
	lightsNR = 0;
}

void SaveSystem::LoadGeneralSettings()
{
	if (!fileLoaded)
	{
		inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/GeneralSettings.txt"));
		std::string path = "";
		int val = 0;
		if (inf.good())
		{
			if (inf.is_open())
			{
				while (inf >> path >> val)
				{
					if (path == "GameObjectsSize=")
					{
						ObjectsCount = val;
					}
					if (path == "lightsNR=")
					{
						lightsNR = val;
					}
					if (path == "EnabledLights=")
					{
						enabledLights = val;
					}
				}
			}


			isObstacleSave.resize(ObjectsCount);
			isWalkableSave.resize(ObjectsCount);
			isAnimatedSave.resize(ObjectsCount);
			isInstanceObjSave.resize(ObjectsCount);
			isPBRSave.resize(ObjectsCount);
			isEmptyTargetSave.resize(ObjectsCount);
			isTexturedSave.resize(ObjectsCount);
			isObjectDeleted.resize(ObjectsCount);


			for (int i = 0; i < ObjectsCount; i++)
			{

				inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/GeneralSettings.txt"));
				std::string path = "";
				std::string val = "";


				if (inf.is_open())
				{
					while (inf >> path >> val)
					{

						if (path == ("GameObject" + std::to_string(i) + ".FilePath="))
						{
							if (ObjectsFilePaths.size() < ObjectsCount)
								ObjectsFilePaths.push_back(val);
						}
					}


				}
			}
		}
		else
		{
			outfile = std::ofstream("Files/File" + std::to_string(currentLevel) + "/GeneralSettings.txt");
			outfile.close();
		}


	}
}

void SaveSystem::SaveGeneralSettings(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights,EnvironmentProbeMap& probeMap,XMFLOAT4& centerColor, XMFLOAT4& apexColor)
{

	outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/GeneralSettings.txt"));

	if (outfile.is_open())
	{
		outfile << "GameObjectsSize= " << gameObjects.size() << "\n";
		outfile << "lightsNR= " << pointLights.size() << "\n";
		//outfile << "EnabledLights= " << enableLights << "\n";

		for (int i = 0; i < gameObjects.size(); i++)
		{
			std::string _path;
		
			size_t npos = gameObjects[i]->filePath.find("Data");
			_path = gameObjects[i]->filePath.substr(npos);
			
			outfile << "GameObject" + std::to_string(i) + ".FilePath= " << ".//" << _path << "\n";
		}
	}


	outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/SkySettings.txt"));
	if (outfile.is_open())
	{
		outfile << "probeMapPosX= " << probeMap.pos.x << "\n";
		outfile << "probeMapPosY= " << probeMap.pos.y << "\n";
		outfile << "probeMapPosZ= " << probeMap.pos.z << "\n";

		outfile << "centerColorX= " << centerColor.x << "\n";
		outfile << "centerColorY= " << centerColor.y << "\n";
		outfile << "centerColorZ= " << centerColor.z << "\n";
		outfile << "centerColorW= " << centerColor.w << "\n";

		outfile << "apexColorX= " << apexColor.x << "\n";
		outfile << "apexColorY= " << apexColor.y << "\n";
		outfile << "apexColorZ= " << apexColor.z << "\n";
		outfile << "apexColorW= " << apexColor.w << "\n";
	}


}

void SaveSystem::LoadSkySettings(EnvironmentProbeMap& probeMap, XMFLOAT4& centerColor, XMFLOAT4& apexColor)
{




	inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/SkySettings.txt"));
	std::string path1 = "";
	float val1 = 0.0;
	if (inf.good())
	{
		if (inf.is_open())
		{
			while (inf >> path1 >> val1)
			{
				if (path1 == "probeMapPosX=")
				{
					probeMap.pos.x = (float)val1;
				}
				if (path1 == "probeMapPosY=")
				{
					probeMap.pos.y = (float)val1;
				}
				if (path1 == "probeMapPosZ=")
				{
					probeMap.pos.z = (float)val1;
				}
				if (path1 == "centerColorX=")
				{
					centerColor.x = (float)val1;
				}
				if (path1 == "centerColorY=")
				{
					centerColor.y = (float)val1;
				}
				if (path1 == "centerColorZ=")
				{
					centerColor.z = (float)val1;
				}
				if (path1 == "centerColorW=")
				{
					centerColor.w = (float)val1;
				}
				if (path1 == "apexColorX=")
				{
					apexColor.x = (float)val1;
				}
				if (path1 == "apexColorY=")
				{
					apexColor.y = (float)val1;
				}
				if (path1 == "apexColorZ=")
				{
					apexColor.z = (float)val1;
				}
				if (path1 == "apexColorW=")
				{
					apexColor.w = (float)val1;
				}
			}

		}
	}
	else
	{
		outfile = std::ofstream("Files/File" + std::to_string(currentLevel) + "/SkySettings.txt");
		outfile.close();
	}
}

void SaveSystem::LoadGameObjectsData(std::vector<GameObject*>& gameObjects)
{
	if (!fileLoaded)
	{

		//LoadGeneralSettings();
		for (int i = 0; i < ObjectsCount; ++i)
		{
			inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
			std::string path = "";
			float val = 0;
			if (inf.good())
			{

				if (inf.is_open())
				{

					while (inf >> path >> val)
					{
						if (path == "attachedObjectNum=")
						{
							gameObjects[i]->list_box_selected_obj = (int)val;
						}
						if (path == "IsAttachedToObject=")
						{
							gameObjects[i]->isAttachedToObj = val;
						}
						if (path == "InstancedObject=")
						{
							gameObjects[i]->model.isInstancedObject = val;
						}
						if (path == "Transparent=")
						{
							gameObjects[i]->isTransparent = val;
						}
						if (path == "Deleted=")
						{
							gameObjects[i]->model.deleted = val;
						}

						if (path == "isPlayer=")
						{
							gameObjects[i]->isPlayer = val;
						}
						if (path == "isCharacter=")
						{
							gameObjects[i]->phsxObj.isCharacter = val;
						}
						if (path == "isAI=")
						{
							gameObjects[i]->isAI = val;
						}
						if (path == "AttachToCamera=")
						{
							gameObjects[i]->attachToCamera = val;
						}

						if (path == "PosX=")
						{
							gameObjects[i]->pos.x = val;
						}
						if (path == "PosY=")
						{
							gameObjects[i]->pos.y = val;

						}
						if (path == "PosZ=")
						{
							gameObjects[i]->pos.z = val;
						}


						if (path == "ModelPosX=")
						{
							gameObjects[i]->translateModel.x = val;
						}
						if (path == "ModelPosY=")
						{
							gameObjects[i]->translateModel.y = val;
						}
						if (path == "ModelPosZ=")
						{
							gameObjects[i]->translateModel.z = val;
						}


						if (path == "ScaleX=")
						{
							gameObjects[i]->scale.x = val;
						}
						if (path == "ScaleY=")
						{
							gameObjects[i]->scale.y = val;
						}
						if (path == "ScaleZ=")
						{
							gameObjects[i]->scale.z = val;
						}

						if (path == "RotX=")
						{
							gameObjects[i]->rot.x = val;
						}
						if (path == "RotY=")
						{
							gameObjects[i]->rot.y = val;
						}
						if (path == "RotZ=")
						{
							gameObjects[i]->rot.z = val;
						}
						if (path == "PhysXScaleX=")
						{
							gameObjects[i]->physxScale.x = val;
						}
						if (path == "PhysXScaleY=")
						{
							gameObjects[i]->physxScale.y = val;
						}
						if (path == "PhysXScaleZ=")
						{
							gameObjects[i]->physxScale.z = val;
						}


						if (path == "PhysXRotX=")
						{
							gameObjects[i]->phsxObj.updateRot.x = val;
						}
						if (path == "PhysXRotY=")
						{
							gameObjects[i]->phsxObj.updateRot.y = val;
						}
						if (path == "PhysXRotZ=")
						{
							gameObjects[i]->phsxObj.updateRot.z = val;
						}
						if (path == "PhysXRotW=")
						{
							gameObjects[i]->phsxObj.updateRot.w = val;
						}

						if (path == "Radius=")
						{
							gameObjects[i]->radius = val;
						}
						if (path == "Mass=")
						{
							gameObjects[i]->phsxObj.mass = val;
						}
						if (path == "convexMeshDetail=")
						{
							gameObjects[i]->phsxObj.convexMeshDetail = (int)val;
						}
						if (path == "triangleMeshStride=")
						{
							gameObjects[i]->phsxObj.triangleMeshStride = (int)val;
						}
						if (path == "FrustumEnabled=")
						{
							gameObjects[i]->frustumEnable = val;
						}
						if (path == "CollisionType=")
						{
							gameObjects[i]->phsxObj.physXShapeType = (PhysXObject::PhysXShapeType)val;
						}
						if (path == "FrustumScaleX=")
						{
							gameObjects[i]->frustumScale.x = val;
						}
						if (path == "FrustumScaleY=")
						{
							gameObjects[i]->frustumScale.y = val;
						}
						if (path == "FrustumScaleZ=")
						{
							gameObjects[i]->frustumScale.z = val;
						}


					}

				}
			}
			else
			{
				outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
				outfile.close();
			}
		}
	}
	fileLoaded = true;



}

void SaveSystem::SaveGameObjectsData(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, EnvironmentProbeMap& probeMap, XMFLOAT4& centerColor, XMFLOAT4& apexColor)
{
	//if (ImGui::Button("SAVE"))
	//{
		SaveGeneralSettings(gameObjects, pointLights, probeMap, centerColor, apexColor);
		//SaveSunLightSettings();
		SaveLightSettings(pointLights);
		for (int i = 0; i < gameObjects.size(); ++i)
		{
			outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());

			if (outfile.is_open())
			{
				outfile << "attachedObjectNum= " << gameObjects[i]->list_box_selected_obj << "\n";
				outfile << "isWalkable= " << gameObjects[i]->isWalkable << "\n";
				outfile << "isObstacle= " << gameObjects[i]->isObstacle << "\n";
				outfile << "IsAttachedToObject= " << gameObjects[i]->isAttachedToObj << "\n";
				outfile << "InstancedObject= " << gameObjects[i]->model.isInstancedObject << "\n";
				outfile << "Transparent= " << gameObjects[i]->isTransparent << "\n";
				outfile << "isPlayer= " << gameObjects[i]->isPlayer << "\n";
				outfile << "isCharacter= " << gameObjects[i]->phsxObj.isCharacter << "\n";
				outfile << "isAI= " << gameObjects[i]->isAI << "\n";
				outfile << "AttachToCamera= " << gameObjects[i]->attachToCamera << "\n";
		

				if (gameObjects[i]->phsxObj.physXShapeType == PhysXObject::PhysXShapeType::eDoor)
				{
					outfile << "PosX= " << gameObjects[i]->phsxObj.jointPos.x << "\n";
					outfile << "PosY= " << gameObjects[i]->phsxObj.jointPos.y << "\n";
					outfile << "PosZ= " << gameObjects[i]->phsxObj.jointPos.z << "\n";
				}
				else
				{
					outfile << "PosX= " << gameObjects[i]->pos.x << "\n";
					outfile << "PosY= " << gameObjects[i]->pos.y << "\n";
					outfile << "PosZ= " << gameObjects[i]->pos.z << "\n";
				}
					


				outfile << "ModelPosX= " << gameObjects[i]->translateModel.x << "\n";
				outfile << "ModelPosY= " << gameObjects[i]->translateModel.y << "\n";
				outfile << "ModelPosZ= " << gameObjects[i]->translateModel.z << "\n";



				outfile << "ScaleX= " << gameObjects[i]->scale.x << "\n";
				outfile << "ScaleY= " << gameObjects[i]->scale.y << "\n";
				outfile << "ScaleZ= " << gameObjects[i]->scale.z << "\n";

	

				outfile << "RotX= " << gameObjects[i]->rot.x << "\n";
				outfile << "RotY= " << gameObjects[i]->rot.y << "\n";
				outfile << "RotZ= " << gameObjects[i]->rot.z << "\n";

				outfile << "PhysXScaleX= " << gameObjects[i]->physxScale.x << "\n";
				outfile << "PhysXScaleY= " << gameObjects[i]->physxScale.y << "\n";
				outfile << "PhysXScaleZ= " << gameObjects[i]->physxScale.z << "\n";

				outfile << "PhysXRotX= " << gameObjects[i]->phsxObj.updateRot.x << "\n";
				outfile << "PhysXRotY= " << gameObjects[i]->phsxObj.updateRot.y << "\n";
				outfile << "PhysXRotZ= " << gameObjects[i]->phsxObj.updateRot.z << "\n";
				outfile << "PhysXRotW= " << gameObjects[i]->phsxObj.updateRot.w << "\n";

			


				outfile << "Radius= " << gameObjects[i]->radius << "\n";
				outfile << "Mass= " << gameObjects[i]->phsxObj.mass << "\n";
				outfile << "convexMeshDetail= " << gameObjects[i]->phsxObj.convexMeshDetail << "\n";
				outfile << "triangleMeshStride= " << gameObjects[i]->phsxObj.triangleMeshStride << "\n";

				outfile << "FrustumEnabled= " << gameObjects[i]->frustumEnable << "\n";
				
				outfile << "CollisionType= " << (int)gameObjects[i]->phsxObj.physXShapeType << "\n";

				
				outfile << "FrustumScaleX= " << gameObjects[i]->frustumScale.x << "\n";
				outfile << "FrustumScaleY= " << gameObjects[i]->frustumScale.y << "\n";
				outfile << "FrustumScaleZ= " << gameObjects[i]->frustumScale.z << "\n";

				outfile << "IsAnimated= " << gameObjects[i]->isAnimated << "\n";
				outfile << "IsPBR= " << gameObjects[i]->isPBR << "\n";
				outfile << "IsTextured= " << gameObjects[i]->isTextured << "\n";
				outfile << "isEmptyTarget= " << gameObjects[i]->emptyTarget << "\n";
				outfile << "isObjectDeleted= " << gameObjects[i]->model.deleted << "\n";
				outfile << "AnimCount= " << gameObjects[i]->AnimCount << "\n";

				for (int j = 0; j < gameObjects[i]->AnimFiles.size(); ++j)
				{
					std::string _path;

					size_t npos = gameObjects[i]->AnimFiles[j].find("Data");
					_path = gameObjects[i]->AnimFiles[j].substr(npos);

					outfile << "AnimFiles" + std::to_string(j) + "= "<< ".//" << _path << "\n";
				}
				outfile << "Name= " << gameObjects[i]->objectName.data() << "\n";


			}
		}

		OutputDebugStringA("SUCCESSFULLY SAVED!\n");
		outfile.close();
		inf.close();
	//}
}

void SaveSystem::PreLoadSettings()
{
	if (!fileLoaded)
	{

		//LoadGeneralSettings();
		for (int i = 0; i < ObjectsCount; ++i)
		{
			inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
			std::string path = "";
			float val = 0;
			if (inf.good())
			{

				if (inf.is_open())
				{
					while (inf >> path >> val)
					{
						if (path == "isWalkable=")
						{
							isWalkableSave[i] = (int)val;
						}
						if (path == "isObstacle=")
						{
							isObstacleSave[i] = (int)val;
						}
						if (path == "InstancedObject=")
						{
							isInstanceObjSave[i] = (int)val;
						}
						if (path == "IsAnimated=")
						{
							isAnimatedSave[i] = (int)val;
						}
						if (path == "IsPBR=")
						{
							isPBRSave[i] = (int)val;
						}
						if (path == "IsTextured=")
						{
							isTexturedSave[i] = (int)val;
						}
						if (path == "isEmptyTarget=")
						{
							isEmptyTargetSave[i] = (int)val;
						}
						if (path == "isObjectDeleted=")
						{
							isObjectDeleted[i] = (int)val;
						}


					}

				}
			}
			else
			{
				outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
				outfile.close();
			}
		}
	}
}

void SaveSystem::LoadSettings(std::vector<GameObject*>& gameObjects)
{
	if (!fileLoaded)
	{

		//LoadGeneralSettings();
		for (int i = 0; i < ObjectsCount; ++i)
		{
			inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
			std::string path = "";

			float val1 = 0;
			if (inf.good())
			{

				if (inf.is_open())
				{
					while (inf >> path >> val1)
					{

						if (path == "AnimCount=")
						{

							gameObjects[i]->AnimCount = (int)val1;
						}
					}
					inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
					path = "";
					std::string val = "";

					while (inf >> path >> val)
					{

						if (path == ("Name="))
						{
							gameObjects[i]->objectName = val;
						}

						//OutputDebugStringA((path + val +"\n").c_str());
						for (int j = 0; j < gameObjects[i]->AnimCount; j++)
						{
							if (path == ("AnimFiles" + std::to_string(j) + "="))
							{
								//OutputDebugStringA((path + val + "\n").c_str());
								gameObjects[i]->AnimFiles.push_back(val);
							}


						}

					}


				}
			}
			else
			{
				outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/ObjectsData" + "/GameObjectAttribs" + std::to_string(i) + ".txt").c_str());
				outfile.close();
			}
		}
	}
}

void SaveSystem::LoadLightSettings(std::vector<PointLight>& pointLights)
{

	for (int i = 0; i < pointLights.size(); ++i)
	{
		inf = std::ifstream(("Files/File" + std::to_string(currentLevel) + "/lightsData" + "/PointLight" + std::to_string(i) + ".txt").c_str());
		std::string path = "";
		float val = 0;

		if (inf.good())
		{

			if (inf.is_open())
			{
				while (inf >> path >> val)
				{
					if (path == "PosX=")
					{
						pointLights[i].pos.x = val;
					}
					if (path == "PosY=")
					{
						pointLights[i].pos.y = val;
					}
					if (path == "PosZ=")
					{
						pointLights[i].pos.z = val;
					}

					if (path == "DirX=")
					{
						pointLights[i].direction.x = val;
					}
					if (path == "DirY=")
					{
						pointLights[i].direction.y = val;
					}
					if (path == "DirZ=")
					{
						pointLights[i].direction.z = val;
					}

					if (path == "ColorX=")
					{
						pointLights[i].lightColor.x = val;
					}
					if (path == "ColorY=")
					{
						pointLights[i].lightColor.y = val;
					}
					if (path == "ColorZ=")
					{
						pointLights[i].lightColor.z = val;
					}
					if (path == "nearZ=")
					{
						pointLights[i].nearZ = val;
					}
					if (path == "farZ=")
					{
						pointLights[i].farZ = val;
					}
					if (path == "fov=")
					{
						pointLights[i].fov = val;
					}
					if (path == "LightStrenth=")
					{
						pointLights[i].lightStrenth = val;
					}
					if (path == "lightAttenuation=")
					{
						pointLights[i].lightAttenuation = val;
					}
					if (path == "radius=")
					{
						pointLights[i].radius = val;
					}
					if (path == "cutOff=")
					{
						pointLights[i].cutOff = val;
					}
					if (path == "enabled=")
					{
						pointLights[i].isLightEnabled = val;
					}
					if (path == "castShadow=")
					{
						pointLights[i].bShadow = val;
					}
					if (path == "lightType=")
					{
						pointLights[i].lightType = val;
					}
					if (path == "isAttached=")
					{
						pointLights[i].isAttached = val;
					}
				}

			}
		}
		else
		{
			outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/lightsData" + "/PointLight" + std::to_string(i) + ".txt").c_str());
			outfile.close();
		}
	}


}

void SaveSystem::SaveLightSettings(std::vector<PointLight>& pointLights)
{
	for (int i = 0; i < pointLights.size(); ++i)
	{
		outfile = std::ofstream(("Files/File" + std::to_string(currentLevel) + "/lightsData" + "/PointLight" + std::to_string(i) + ".txt").c_str());

		if (outfile.is_open())
		{
			outfile << "PosX= " << pointLights[i].pos.x << "\n";
			outfile << "PosY= " << pointLights[i].pos.y << "\n";
			outfile << "PosZ= " << pointLights[i].pos.z << "\n";

			outfile << "DirX= " << pointLights[i].direction.x << "\n";
			outfile << "DirY= " << pointLights[i].direction.y << "\n";
			outfile << "DirZ= " << pointLights[i].direction.z << "\n";



			outfile << "ColorX= " << pointLights[i].lightColor.x << "\n";
			outfile << "ColorY= " << pointLights[i].lightColor.y << "\n";
			outfile << "ColorZ= " << pointLights[i].lightColor.z << "\n";

			outfile << "nearZ= " << pointLights[i].nearZ << "\n";
			outfile << "farZ= " << pointLights[i].farZ << "\n";
			outfile << "fov= " << pointLights[i].fov << "\n";
			outfile << "LightStrenth= " << pointLights[i].lightStrenth << "\n";
			outfile << "lightAttenuation= " << pointLights[i].lightAttenuation << "\n";
			outfile << "radius= " << pointLights[i].radius << "\n";
			outfile << "cutOff= " << pointLights[i].cutOff << "\n";
			outfile << "enabled= " << pointLights[i].isLightEnabled << "\n";
			outfile << "castShadow= " << pointLights[i].bShadow << "\n";
			outfile << "lightType= " << pointLights[i].lightType << "\n";
			outfile << "isAttached= " << pointLights[i].isAttached << "\n";
		}
	}
}
