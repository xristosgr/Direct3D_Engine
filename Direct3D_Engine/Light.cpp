#include "Light.h"

Light::Light()
{
	pos = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	scale = DirectX::XMFLOAT3(0.1, 0.1, 0.1);
	rot = DirectX::XMFLOAT3(1.5, 0.0, 0.0);
	lightColor = DirectX::XMFLOAT3(1.0, 1.0, 1.0);
	radius = 1.0f;
}

bool Light::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize("Data/Objects/pbrTest/sphere.gltf", device, deviceContext, cb_vs_vertexshader, false, false, false))
	{
		return false;
	}
	model.LoadModel(model._filepath, false);

	return true;
}

void Light::DrawGui(std::string name)
{
	ImGui::Checkbox("Enable", &isLightEnabled);

	ImGui::Checkbox("RenderLight", &bRenderModel);

	//ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	//ImGui::DragFloat3("Scale", &scale.x, 0.005f);

	ImGui::DragFloat3("lightColor", &lightColor.x, 0.005f);
	ImGui::DragFloat("light strength", &lightStrenth, 0.005f, 0.0f, 100.0f);
	ImGui::DragFloat("light attenuation", &lightAttenuation, 0.005f, 0.0f, 100.0f);
	ImGui::DragFloat("radius", &radius, 0.05f);
		
	ImGui::NewLine();
}

void Light::Draw(XMMATRIX& viewMatrix, XMMATRIX projectionMatrix)
{
	XMMATRIX scale;
	XMMATRIX rotate;
	XMMATRIX translate;
	XMMATRIX worldMatrix;

	scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	translate = XMMatrixTranslation(pos.x, pos.y, pos.z);

	worldMatrix = (scale * rotate) * translate;

	model.Draw(worldMatrix, viewMatrix, projectionMatrix, false);
}



SunLight::SunLight()
{
	//obj.pos = DirectX::XMFLOAT3(0, 7, 0);
	//obj.scale = DirectX::XMFLOAT3(0.4, 0.4, 0.4);
	//obj.rot = DirectX::XMFLOAT3(1.5, 0.0, 0.0);
	//dirOffset = DirectX::XMFLOAT3(0.0, 9.300, -26.700);
	//
	//for (int i = 0; i < 4; i++)
	//{
	//	posOffset[i] = DirectX::XMFLOAT3(-1.065, 10.220, 25.0f);
	//
	//}
	//
	//for (int i = 0; i < 4; i++)
	//{
	//	orthoNear[i] = 0.001f;
	//	orthoFar[i] = 100.0f;
	//	orthoX[i] = 30;
	//	orthoY[i] = 30;
	//}
}

bool SunLight::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	//bool textureLoaded = false;
	////sunObject.Initialize("Data/Objects/pbrTest/sphere.gltf", device, deviceContext, cb_vs_vertexshader, false, false,false, false);
	//if (!model.Initialize("Data/Objects/pbrTest/sphere.gltf", device, deviceContext, cb_vs_vertexshader,false,false, false))
	//{
	//	return false;
	//}
	//sunObject.scale = XMFLOAT3(40,40, 40);
	//sunPosOffset = XMFLOAT3(0, 300, 1200);

	return true;
}

void SunLight::DrawGui(std::string name)
{
	//ImGui::Begin((const char*)name.c_str());
	//__super::DrawGui(name);

	//if (ImGui::CollapsingHeader((name+std::to_string(count)).c_str()))
	//{
	//
	//	windowActive = true;
	//	
	//	sunObject.DrawGui("SunObject");
	//	ImGui::DragFloat3("sunPosOffset", &sunPosOffset.x, 0.005f);
	//	ImGui::Begin((const char*)name.c_str());
	//	ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	//	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	//	for (int i = 0; i < 4; i++)
	//	{
	//		ImGui::DragFloat3(("posOffset"+std::to_string(i)).c_str(), &posOffset[i].x, 0.005f);
	//	}
	//	
	//	ImGui::DragFloat3("Scale", &scale.x, 0.005f);
	//
	//	ImGui::DragFloat3("lightColor", &lightColor.x, 0.005f);
	//	ImGui::DragFloat("specularPower", &specularPower, 0.05f);
	//
	//	//ImGui::DragFloat("light strength", &lightStrenth, 0.005f, 0.0f, 100.0f);
	//	ImGui::DragFloat3("direction", &direction.x, 0.05f);
	//	ImGui::DragFloat3("dirOffset", &dirOffset.x, 0.05f);
	//
	//
	//	for (int i = 0; i < 4; i++)
	//	{
	//		ImGui::DragInt(("orthoX"+std::to_string(i)).c_str(), &this->orthoX[i], 1.0f, 1, 1000);
	//		ImGui::DragInt(("orthoY" + std::to_string(i)).c_str(), &this->orthoY[i], 1, 1, 1000);
	//		ImGui::DragFloat(("orthoNear" + std::to_string(i)).c_str(), &this->orthoNear[i], 0.05f, 0.001f, 100.0f);
	//		ImGui::DragFloat(("orthoFar" + std::to_string(i)).c_str(), &this->orthoFar[i], 0.05f, 1.0f, 10000.0f);
	//	}
	//
	//
	//	if (windowActive)
	//		ImGui::End();
	//}
	//else
	//{
	//	windowActive = false;
	//}
	//ImGui::NewLine();
}

void SunLight::SetupCamera()
{
	//for (int i = 0; i < 4; i++)
	//{
	//	if(i>0)
	//		camera[i].SetPosition(pos.x , pos.y * (i+1), pos.z);
	//	else
	//		camera[i].SetPosition(pos.x, pos.y , pos.z);
	//	camera[i].SetLookAtPos(XMFLOAT3(direction.x + dirOffset.x, direction.y + dirOffset.y, direction.z + dirOffset.z));
	//	//camera[i].PerspectiveFov(90.0f, static_cast<float>(windowWidth / windowHeight)*2, 1.f, 100000.0f);
	//
	//	lightViewMatrix[i] = camera[i].GetViewMatrix();
	//	lightProjectionMatrix[i] = camera[i].GetProjectionMatrix();
	//
	//	camera[i].OrthographicFov(orthoX[i], orthoY[i], orthoNear[i], orthoFar[i]);
	//}




}

//Camera* SunLight::GetCamera()
//{
//	return &camera;
//}

void SunLight::UpdateCamera()
{


	//for (int i = 0; i < 4; i++)
	//{
	//	camera[i].SetLookAtPos(XMFLOAT3(direction.x + dirOffset.x, direction.y + dirOffset.y, direction.z + dirOffset.z));
	//
	//	camera[i].SetPosition(pos.x + posOffset[i].x, pos.y+ posOffset[i].y, pos.z + posOffset[i].z);
	//
	//
	//	lightViewMatrix[i] = camera[i].GetViewMatrix();
	//	lightProjectionMatrix[i] = camera[i].GetProjectionMatrix();
	//
	//
	//	camera[i].OrthographicFov(orthoX[i], orthoY[i], orthoNear[i], orthoFar[i]);
	//}

}






PointLight::PointLight()
{
	pos = DirectX::XMFLOAT3(1.8, 0.0, 0.0);
	scale = DirectX::XMFLOAT3(0.05, 0.05, 0.05);
	rot = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	posOffset = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	lightColor = DirectX::XMFLOAT3(10.0, 1.0, 1.0);
	fov = 90.0f;
	lightStrenth = 10.0f;
	radius = 1.0f;
	lightAttenuation = 1.0f;
	dimensions = 2;
	cutOff = 0.1f;
	lightType = 0.0f;
	isAttached = false;
	offset = DirectX::XMFLOAT3(0.0, 0.0, 0.0);
	//direction = DirectX::XMFLOAT3(-0.8f, -13.7f, -12.75f);
	//direction[0] = DirectX::XMFLOAT3(1.0f, 1000.0f, 1.0f);
	//direction[1] = DirectX::XMFLOAT3(1.0f, -1000.0f, 1.0f);
	//direction[2] = DirectX::XMFLOAT3(1000.0f, 1.0f, 1.0f);
	//direction[3] = DirectX::XMFLOAT3(-1000.0f, 1.0f, 1.0f);
	//direction[4] = DirectX::XMFLOAT3(1.0f, 1.0f, -1000.0f);
	//direction[5] = DirectX::XMFLOAT3(1.0f, 1.0f, 1000.0f);
}

bool PointLight::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	timer.Start();
	if (!model.Initialize("Data/Objects/pbrTest/sphere.gltf", device, deviceContext, cb_vs_vertexshader, false, false, false))
	{
		return false;
	}
	model.LoadModel(model._filepath, false);

	return true;
}

void PointLight::DrawGui(std::string name)
{
	ImGui::Checkbox("isAttached", &isAttached);
	ImGui::Checkbox("Enable", &isLightEnabled);
	ImGui::Checkbox("CastShadow", &bShadow);
	ImGui::Checkbox("RenderLight", &bRenderModel);
	
	ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
	ImGui::DragFloat3("Translation", &pos.x, 0.005f);
	//ImGui::DragFloat3("posOffset", &posOffset.x, 0.005f);
	ImGui::DragFloat3("Scale", &scale.x, 0.005f);

	ImGui::DragFloat3("lightColor", &lightColor.x, 0.005f);
	ImGui::DragFloat("light strength", &lightStrenth, 0.005f, 0.0f, 100.0f);
	ImGui::DragFloat("light attenuation", &lightAttenuation, 0.005f, 0.0f, 100.0f);
	
	ImGui::DragFloat3("direction", &direction.x, 0.05f);
	ImGui::DragFloat3("offset", &offset.x, 0.05f);
	ImGui::DragFloat("radius", &radius, 0.05f);
	ImGui::DragFloat("cutOff", &cutOff, 0.01f);
	ImGui::InputFloat("lightType", &lightType);

	ImGui::NewLine();
	ImGui::DragFloat("nearZ", &nearZ, 0.05f);
	ImGui::DragFloat("farZ", &farZ, 0.05f);
	ImGui::DragFloat("fov" , &fov, 0.05f);
	ImGui::DragFloat("dimmensions", &dimensions, 0.05f);
	
	ImGui::NewLine();
}

void PointLight::Draw(XMMATRIX& viewMatrix, XMMATRIX projectionMatrix)
{
	XMMATRIX scale;
	XMMATRIX rotate;
	XMMATRIX translate;
	XMMATRIX worldMatrix;

	scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	translate = XMMatrixTranslation(pos.x, pos.y, pos.z);

	worldMatrix = (scale * rotate) * translate;

	int rVal = 0;

	rVal = rand();

	//if (rVal % 10 == 0)
	//{
	//	isLightEnabled = true;
	//}
	//else if(rVal % 15 == 0)
	//{
	//	isLightEnabled = false;
	//}
	

	if (isLightEnabled)
		model.Draw(worldMatrix, viewMatrix, projectionMatrix, false);

}

void PointLight::SetupCamera(int windowWidth, int windowHeight)
{
		//fov = 90.0f;
		camera.SetPosition(pos.x, pos.y, pos.z);
		camera.SetLookAtPos(direction);
		camera.PerspectiveFov(90.0f, 1, 0.1f, 60.0f);

		lightViewMatrix = camera.GetViewMatrix();
		lightProjectionMatrix = camera.GetProjectionMatrix();




}


void PointLight::UpdateCamera()
{
	//pos.x += sin(timer.GetMilisecondsElapsed()*0.1f / 1000.0f)*0.02f;

		camera.SetPosition(pos);
		//camera[i].pos = pos;
		camera.PerspectiveFov(fov, dimensions, nearZ, farZ);
		//camera.SetLookAtPos(XMFLOAT3(direction.x + pos.x, direction.y + pos.y, direction.z + pos.z));
		camera.SetLookAtPos(direction);
		//camera[i].SetPosition(pos.x, pos.y, pos.z);
		lightViewMatrix = camera.GetViewMatrix();
		lightProjectionMatrix = camera.GetProjectionMatrix();
		
	


}




