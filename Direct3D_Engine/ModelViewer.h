#pragma once
#include "GameObject.h"
#include "ImGui/ImGuiFileBrowser.h"

class ModelViewer
{
public:
	ModelViewer();
	void EditModel(GameObject& gameObject,Camera& camera);

private:
	int animIndex = 0;
	imgui_addons::ImGuiFileBrowser file_dialog;
};

