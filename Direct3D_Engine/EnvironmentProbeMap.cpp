#include "EnvironmentProbeMap.h"

EnvironmentProbeMap::EnvironmentProbeMap()
{


	//direction[0] = DirectX::XMFLOAT3(1.0f, 1000.0f, 1.0f);
	//direction[1] = DirectX::XMFLOAT3(1.0f, -1000.0f, 1.0f);
	//direction[2] = DirectX::XMFLOAT3(1000.0f, 1.0f, 1.0f);
	//direction[3] = DirectX::XMFLOAT3(-1000.0f, 1.0f, 1.0f);
	//direction[4] = DirectX::XMFLOAT3(1.0f, 1.0f, -1000.0f);
	//direction[5] = DirectX::XMFLOAT3(1.0f, 1.0f, 1000.0f);


	direction[0] = DirectX::XMFLOAT3(-1000.0f, 0.02f, 1.0f);
	direction[1] = DirectX::XMFLOAT3(1000.0f, -0.02f, 1.0f);
	direction[2] = DirectX::XMFLOAT3(1.0f, 1000.0f, 1.0f);
	direction[3] = DirectX::XMFLOAT3(0.0f, -1000.0f, 0.0f);
	direction[4] = DirectX::XMFLOAT3(1.0f, 0.02f, -1000.0f);
	direction[5] = DirectX::XMFLOAT3(1.0f, -0.02f, 1000.0f);

	pos = DirectX::XMFLOAT3(0, 0.0f, 0.0f);
	scale = DirectX::XMFLOAT3(0.4, 0.4, 0.4);
	rot = DirectX::XMFLOAT3(1.5, 0.0, 0.0);
}

bool EnvironmentProbeMap::InitializeProbe(ID3D11Device* device, ID3D11DeviceContext* deviceContex, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize("Data/Objects/pbrTest/sphere.gltf", device, deviceContex, cb_vs_vertexshader, false, false, false))
	{
		return false;
	}
	model.LoadModel("Data/Objects/pbrTest/sphere.gltf", false);
	return true;
	
}
void EnvironmentProbeMap::SetupCamera(int& windowWidth, int& windowHeight)
{

	for (int i = 0; i < 6; i++)
	{

		camera[i].SetPosition(pos.x, pos.y, pos.z);
		camera[i].SetLookAtPos(XMFLOAT3(direction[i].x, direction[i].y, direction[i].z));
		camera[i].PerspectiveFov(90.0f,1, 0.1f, 100000.0f);

		viewMatrices[i] = camera[i].GetViewMatrix();
		projectionMatrices[i] = camera[i].GetProjectionMatrix();

	}
}

void EnvironmentProbeMap::UpdateCamera(int& windowWidth, int& windowHeight,float fov)
{


	for (int i = 0; i < 6; i++)
	{
		camera[i].SetLookAtPos(XMFLOAT3(direction[i].x + pos.x, direction[i].y + pos.y, direction[i].z + pos.z));

		camera[i].SetPosition(pos.x, pos.y, pos.z);
		camera[i].PerspectiveFov(fov, 1, 0.1f, 10000.0f);

		viewMatrices[i] = camera[i].GetViewMatrix();
		projectionMatrices[i] = camera[i].GetProjectionMatrix();

	}

}

void EnvironmentProbeMap::DrawGui(std::string name)
{
	if (ImGui::CollapsingHeader((name).c_str()))
	{
		//windowActive = true;
		//if (windowActive)
		//	ImGui::Begin((const char*)name.c_str());
		//ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
		ImGui::DragFloat3("Translation", &pos.x, 0.005f);

		//ImGui::DragFloat3("Scale", &scale.x, 0.005f);

		//for (int i = 0; i < 6; i++)
		//{
		//	ImGui::DragFloat3(("Direction"+std::to_string(i)).c_str(), &direction[i].x, 0.005f);
		//}

		//if (ImGui::Button("Recalculate"))
		//{
		//	recalculate = true;
		//}

		//if (windowActive)
		//	ImGui::End();
	}
	//else
	//{
	//	windowActive = false;
	//}
	ImGui::NewLine();
}
