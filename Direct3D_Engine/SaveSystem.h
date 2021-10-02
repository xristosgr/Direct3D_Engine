#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "GraphicsVariables.h"
#include<string>
#include <fstream>
class SaveSystem
{
public:

	SaveSystem();
	void LoadGeneralSettings();
	void SaveGeneralSettings(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, EnvironmentProbeMap& probeMap, XMFLOAT4& centerColor, XMFLOAT4& apexColor);
	void LoadSkySettings(EnvironmentProbeMap& probeMap, XMFLOAT4& centerColor, XMFLOAT4& apexColor);
	void LoadGameObjectsData(std::vector<GameObject*>& gameObjects);
	void SaveGameObjectsData(std::vector<GameObject*>& gameObjects, std::vector<PointLight>& pointLights, EnvironmentProbeMap& probeMap, XMFLOAT4& centerColor, XMFLOAT4& apexColor);
	void PreLoadSettings();
	void LoadSettings(std::vector<GameObject*>& gameObjects);
	//void LoadSunLightSettings();
	//void SaveSunLightSettings();
	void LoadLightSettings(std::vector<PointLight>& pointLights);
	void SaveLightSettings(std::vector<PointLight>& pointLights);
private:

	bool saveGame = false;
	bool fileLoaded = false;

	std::ifstream inf;
	std::ofstream outfile;



	int currentLevel = 0;
	unsigned int enabledLights = 0;

public:
	int ObjectsCount = 0;
	unsigned int lightsNR = 0;


	std::vector<std::string> ObjectsFilePaths;
	std::vector<bool> isAnimated;
	std::vector<bool> isPBR;
	std::vector<bool> isEmptyTarget;
	std::vector<bool> isTextured;
	std::vector<bool> isAnimatedSave;
	std::vector<bool> isInstanceObjSave;
	std::vector<bool> isPBRSave;
	std::vector<bool> isEmptyTargetSave;
	std::vector<bool> isObjectDeleted;
	std::vector<bool> isTexturedSave;
	std::vector<bool> isWalkableSave;
	std::vector<bool> isObstacleSave;
};

