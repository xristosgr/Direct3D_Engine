#include "ModelViewer.h"
#include <fstream>


ModelViewer::ModelViewer()
{

}

void ModelViewer::EditModel(GameObject& gameObject, Camera& camera)
{
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	
	ImGui::Begin("Animation track");
	ImGui::InputInt("animIndex", &animIndex);
	ImGui::Text(("Anim Count: " + std::to_string(gameObject.AnimFiles.size()+1)).c_str());
	ImGui::Text(("Anim0: " + gameObject.filePath).c_str());
	for (int animIndex = 0; animIndex < gameObject.AnimFiles.size(); ++animIndex)
	{
		ImGui::Text(("Anim" + std::to_string(animIndex + 1) + ": " + gameObject.AnimFiles[animIndex]).c_str());
	}
	ImGui::Checkbox("stepAnimation", &gameObject.model.stepAnimation);
	ImGui::DragFloat("currentAnimFrame", &gameObject.model.currentAnimTime, 1.0f);
	ImGui::InputInt("animLevel", &gameObject.model.animLevel);
	if (gameObject.isAnimated)
	{
		gameObject.model.SetAnimIndex(animIndex);
		gameObject.model.Update();

		//static bool animate = true;
		//ImGui::Checkbox("Animate", &animate);
		std::vector<float> keyFrames;
		for (int i = 0; i <= (int)gameObject.model.animDuration; ++i)
		{
			if (!gameObject.model.stepAnimation)
			{
				if (gameObject.model.mAnimationTime >= i - 6 && gameObject.model.mAnimationTime <= i + 6)
					keyFrames.push_back((float)i);
				else
					keyFrames.push_back(0.0f);
			}
			else
			{
				if (gameObject.model.currentAnimTime >= i-6 && gameObject.model.currentAnimTime <= i + 6)
					keyFrames.push_back((float)i);
				else
					keyFrames.push_back(0.0f);
			}
			
		}
		char overlay[32];

		if (!gameObject.model.stepAnimation)
		{
			sprintf_s(overlay, "key frame %f", gameObject.model.mAnimationTime);
			ImGui::PlotHistogram("Animation graph", keyFrames.data(), keyFrames.size(), 0, NULL, 0.0f, 1.0f, ImVec2(0, 60));
		}
		else
		{
			sprintf_s(overlay, "key frame %f", gameObject.model.currentAnimTime);
			ImGui::PlotHistogram("Animation graph", keyFrames.data(), keyFrames.size(), 0, NULL, 0.0f, 1.0f, ImVec2(0, 60));
		}
		
		

		if (ImGui::CollapsingHeader("Animation Params"))
		{
			std::fstream f;

			bool open = false, save = false;

			//ImGui::SameLine();
			if (ImGui::BeginMenu("Files"))
			{
				if (ImGui::MenuItem("Open", NULL))
					open = true;

				ImGui::EndMenu();
			}

			//Remember the name to ImGui::OpenPopup() and showFileDialog() must be same...
			if (open)
				ImGui::OpenPopup("Open File");
		
			if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(400, 200), "*.*,.obj,.dae,.gltf,.glb,.fbx"))
			{
				std::cout << file_dialog.selected_fn << std::endl;      // The name of the selected file or directory in case of Select Directory dialog mode
				std::cout << file_dialog.selected_path << std::endl;    // The absolute path to the selected file
				f = std::fstream(file_dialog.selected_path.c_str());
				if (f.good())
					gameObject.inName = file_dialog.selected_path;

				//OutputDebugStringA(("NAME = " + inName + "\n").c_str());
			}

			ImGui::Text(gameObject.inName.c_str());


			
			if (ImGui::Button("AddAnim"))
			{
				if (f.good())
				{
					gameObject.AnimCount++;
					gameObject.model.LoadModel(gameObject.inName, &gameObject.texturesLoaded);
					gameObject.AnimFiles.push_back(gameObject.inName);
				}


			}
			if (!f.good())
			{
				ImGui::Text("Invalid filepath!");
			}

			ImGui::DragFloat("Animation Speed", &gameObject.model.animSpeed, 0.005f);
		}
		
	}
	//ImGui::ShowDemoWindow();
	
}

