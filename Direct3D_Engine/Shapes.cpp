#include "Shapes.h"

Shapes::Shapes()
{

	viewMatrices[0] = XMFLOAT3(1.0f, 0.0f, 0.0f);
	viewMatrices[1] = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	viewMatrices[2] = XMFLOAT3(0.0f, 1000.0f, 0.0f);
	viewMatrices[3] = XMFLOAT3(0.0f, -1000.0f, 0.0f);
	viewMatrices[4] = XMFLOAT3(0.0f, 0.0f, 1.0f);
	viewMatrices[5] = XMFLOAT3(0.0f, 0.0f, -1.0f);
}

void Shapes::InitRect(const int sizeX, const int sizeY, Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,bool shadowsAlignment)
{
	int sX = sizeX;
	int sY = sizeY;

	scale = DirectX::XMFLOAT3(0.5, 0.5, 1);
	rot = DirectX::XMFLOAT3(0, 0, -1.571);
	pos = DirectX::XMFLOAT3(0, 2.87, 0);


	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	this->device = device;
	this->deviceContext = deviceContext;

	//this->vertices.push_back(Vertex(-sY, -sX, 0.0f, 0.0f));
	//this->vertices.push_back(Vertex(-sY, sX, 1.0f , 0.0f ));
	//this->vertices.push_back(Vertex(sY, sX, 1.0f , 1.0f ));
	//this->vertices.push_back(Vertex(sY, -sX, 0.0f, 1.0f ));

	this->vertices.push_back(Vertex(-sY, -sX, 0.0f, 0.0f));
	this->vertices.push_back(Vertex(-sY, sX, 1.0f, 0.0f));
	this->vertices.push_back(Vertex(sY, sX, 1.0f, 1.0f));
	this->vertices.push_back(Vertex(sY, -sX, 0.0f, 1.0f));

	this->indices.push_back(0);
	this->indices.push_back(1);
	this->indices.push_back(2);
	this->indices.push_back(3);
	this->indices.push_back(0);
	this->indices.push_back(2);

	//mesh = new Mesh(device, deviceContext, this->vertices, transformMatrix);
	mesh = new Mesh(device, deviceContext, this->vertices, this->indices, transformMatrix);
}
void Shapes::InitCube(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, DirectX::XMMATRIX transformMatrix, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, bool visualize)
{

	scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	rot = DirectX::XMFLOAT3(0, 0, 0);
	pos = DirectX::XMFLOAT3(0, 0, 0);

	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	this->device = device;
	this->deviceContext = deviceContext;

	int correction = 1;


	if (visualize)
	{

		this->vertices.push_back(Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f));	 // bottom-left
		this->vertices.push_back(Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f));		// top-right
		this->vertices.push_back(Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f));		 // bottom-right         
		this->vertices.push_back(Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f));		// top-right
		this->vertices.push_back(Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f));	 // bottom-left
		this->vertices.push_back(Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f));	// top-left
		// front face
		this->vertices.push_back(Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f));	// bottom-left
		this->vertices.push_back(Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f));// bottom-right
		this->vertices.push_back(Vertex(1.0f, 1.0f, 1.0f, 1.0f, 1.0f));// top-right
		this->vertices.push_back(Vertex(1.0f, 1.0f, 1.0f, 1.0f, 1.0f));// top-right
		this->vertices.push_back(Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f));// top-left
		this->vertices.push_back(Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f));	// bottom-left
			// left face
		this->vertices.push_back(Vertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f));		// top-right
		this->vertices.push_back(Vertex(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f));	// top-left
		this->vertices.push_back(Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f));		// bottom-left
		this->vertices.push_back(Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f));		// bottom-left
		this->vertices.push_back(Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f));	// bottom-right
		this->vertices.push_back(Vertex(-1.0f, 1.0f, 1.0f, 1.0f, 0.0f));	// top-right
											// right face
		this->vertices.push_back(Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f));// top-left
		this->vertices.push_back(Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f));	// bottom-right
		this->vertices.push_back(Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f));	// top-right         
		this->vertices.push_back(Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f));	 // bottom-right
		this->vertices.push_back(Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f));	 // top-left
		this->vertices.push_back(Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f));	// bottom-left     
										  // bottom face
		this->vertices.push_back(Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f));	 // top-right
		this->vertices.push_back(Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f));	// top-left
		this->vertices.push_back(Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f));	// bottom-left
		this->vertices.push_back(Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f));	// bottom-left
		this->vertices.push_back(Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f));	// bottom-right
		this->vertices.push_back(Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f));		// top-right
											// top face
		this->vertices.push_back(Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f));	 // top-left
		this->vertices.push_back(Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f));	// bottom-right
		this->vertices.push_back(Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f));	// top-right     
		this->vertices.push_back(Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f));	// bottom-right
		this->vertices.push_back(Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f));	// top-left
		this->vertices.push_back(Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f));		// bottom-left      



		mesh = new Mesh(device, deviceContext, this->vertices, transformMatrix);
	}

}

void Shapes::Draw(Camera& camera, ID3D11DepthStencilState* depthStencilState2D,const XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, int& windowWidth,int& windowHeight,float screenNear,float screenFar, bool blenderModel, bool isTextured,bool isPBR)
{
	//if (windowWidth == 1920 && windowHeight == 1080)
	//{
	//	scale = DirectX::XMFLOAT3(5.595f, 9.6f, 1);
	//	pos = DirectX::XMFLOAT3(0, 2.520, 0);
	//}
	camera.OrthographicFov(windowWidth, windowHeight, screenNear, screenFar);
	deviceContext->OMSetDepthStencilState(depthStencilState2D, 0);

	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z );
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX translate = XMMatrixTranslation(pos.x, pos.y, pos.z);


	XMMATRIX worldMatrix = scale * rotate * translate;

	deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());
	this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
	this->cb_vs_vertexshader->data.projectionMatrix = projectionMatrix;
	this->cb_vs_vertexshader->data.worldMatrix =  worldMatrix;
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;
	this->cb_vs_vertexshader->UpdateBuffer();

	mesh->Draw(isTextured, isPBR,false);

}

void Shapes::DrawObject(Camera& camera,const XMMATRIX& viewMatrix, bool blenderModel, bool isTextured)
{


	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX translate = XMMatrixTranslation(pos.x, pos.y, pos.z);


	XMMATRIX worldMatrix = scale * rotate * translate;

	deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());
	this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
	this->cb_vs_vertexshader->data.projectionMatrix = camera.GetProjectionMatrix();
	this->cb_vs_vertexshader->data.worldMatrix = worldMatrix;
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewMatrix * camera.GetProjectionMatrix();
	this->cb_vs_vertexshader->UpdateBuffer();
	mesh->DrawCube(isTextured,false);

}
void Shapes::DrawRect(Camera& camera, const XMMATRIX& viewMatrix, bool blenderModel, bool isTextured)
{
	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX translate = XMMatrixTranslation(pos.x, pos.y, pos.z);


	XMMATRIX worldMatrix = scale * rotate * translate;

	deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());
	this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
	this->cb_vs_vertexshader->data.projectionMatrix = camera.GetProjectionMatrix();
	this->cb_vs_vertexshader->data.worldMatrix = worldMatrix;
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewMatrix * camera.GetProjectionMatrix();
	this->cb_vs_vertexshader->UpdateBuffer();
	mesh->DrawRect(isTextured, false);
}
void Shapes::DrawObject(Camera& camera, const XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, bool blenderModel, bool isTextured)
{


	XMMATRIX scale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	XMMATRIX translate = XMMatrixTranslation(pos.x, pos.y, pos.z);


	XMMATRIX worldMatrix = scale * rotate * translate;

	deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetBuffer().GetAddressOf());
	this->cb_vs_vertexshader->data.viewMatrix = viewMatrix;
	this->cb_vs_vertexshader->data.projectionMatrix = projectionMatrix;
	this->cb_vs_vertexshader->data.worldMatrix = worldMatrix;
	this->cb_vs_vertexshader->data.wvpMatrix = worldMatrix * viewMatrix * projectionMatrix;
	this->cb_vs_vertexshader->UpdateBuffer();
	mesh->DrawCube(isTextured, false);

}
void Shapes::DrawGui(std::string name)
{

	if (ImGui::CollapsingHeader(name.c_str()))
	{
		ImGui::DragFloat3("Scale", &scale.x, 0.005f);
		ImGui::DragFloat3("Rotate", &rot.x, 0.005f);
		ImGui::DragFloat3("Translation", &pos.x, 0.005f);
		ImGui::DragFloat3("Direction", &direction.x, 0.005f);
		//ImGui::DragInt("SelectView", &select, 1,0,5);
	}
	ImGui::NewLine();
}


void Shapes::SetupCamera(int windowWidth, int windowHeight)
{
	camera.SetPosition(pos.x, pos.y, pos.z);
	camera.SetLookAtPos(direction);
	camera.PerspectiveFov(90.0f, static_cast<float>(windowWidth / windowHeight), 1.f, 100000.0f);
	//camera.OrthographicFov(20, 20, 1.0f, 50.0f);
	cameraViewMatrix = camera.GetViewMatrix();
	cameraProjectionMatrix = camera.GetProjectionMatrix();

}


Camera* Shapes::GetCamera()
{
	return &camera;
}

void Shapes::UpdateCamera(int& select)
{
	
	
	if(select == 0)
		camera.SetLookAtPos(viewMatrices[0]);
	else if (select == 1)
		camera.SetLookAtPos(viewMatrices[1]);
	else if (select == 2)
		camera.SetLookAtPos(viewMatrices[2]);
	else if (select == 3)
		camera.SetLookAtPos(viewMatrices[3]);
	else if (select == 4)
		camera.SetLookAtPos(viewMatrices[4]);
	else if (select == 5)
		camera.SetLookAtPos(viewMatrices[5]);

	
	cameraViewMatrix = camera.GetViewMatrix();
	cameraProjectionMatrix = camera.GetProjectionMatrix();
}
