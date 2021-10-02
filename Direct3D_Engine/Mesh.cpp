#include "Mesh.h"

Mesh::Mesh()
{
	deleted = true;
}

Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, std::vector<Texture>& textures, DirectX::XMMATRIX transformMatrix)
{
	this->deviceContext = deviceContext;
	this->transformMatrix = transformMatrix;

	//this->textures = textures;

	this->textures.push_back(textures[0]);
	this->textures.push_back(textures[1]);
	this->textures.push_back(textures[2]);
	this->textures.push_back(textures[3]);
	HRESULT hr = this->vertexBuffer.Initialize(device.Get(), vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");


	hr = this->indexBuffer.Initialize(device.Get(), indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
}

Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, std::vector<VertexBoneData>& bones, DirectX::XMMATRIX transformMatrix)
{
	this->deviceContext = deviceContext;
	this->transformMatrix = transformMatrix;
	HRESULT hr = this->vertexBuffer.Initialize(device.Get(), vertices.data(),vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->boneDataBuffer.Initialize(device.Get(), bones.data(), bones.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexBuffer.Initialize(device.Get(), indices.data(),indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

}
Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices, std::vector<Texture>& textures, std::vector<VertexBoneData>& bones, DirectX::XMMATRIX transformMatrix)
{
	this->deviceContext = deviceContext;
	this->transformMatrix = transformMatrix;
	this->textures.push_back(textures[0]);
	this->textures.push_back(textures[1]);
	this->textures.push_back(textures[2]);
	this->textures.push_back(textures[3]);
	HRESULT hr = this->vertexBuffer.Initialize(device.Get(), vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->boneDataBuffer.Initialize(device.Get(), bones.data(), bones.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexBuffer.Initialize(device.Get(), indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

}
Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, DirectX::XMMATRIX transformMatrix)
{
	this->deviceContext = deviceContext;
	this->transformMatrix = transformMatrix;
	HRESULT hr = this->vertexBuffer.Initialize(device.Get(), vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

}

Mesh::Mesh(Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& deviceContext, std::vector<Vertex>& vertices, std::vector<UINT32>& indices,  DirectX::XMMATRIX transformMatrix)
{
	this->deviceContext = deviceContext;
	this->transformMatrix = transformMatrix;
	HRESULT hr = this->vertexBuffer.Initialize(device.Get(), vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");


	hr = this->indexBuffer.Initialize(device.Get(), indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");

}


void Mesh::Draw(bool isTextured,bool isPBR,bool isInstanced)
{
	if (!deleted)
	{
		UINT offset = 0;

		if (textures.size() > 0)
		{
			//if (isTextured)
			//{
				this->deviceContext->PSSetShaderResources(0, 1, textures[0].GetTextureResourceViewAddress());

				//if (isPBR)
				//{
					this->deviceContext->PSSetShaderResources(1, 1, textures[1].GetTextureResourceViewAddress());
					this->deviceContext->PSSetShaderResources(2, 1, textures[2].GetTextureResourceViewAddress());
					this->deviceContext->PSSetShaderResources(3, 1, textures[3].GetTextureResourceViewAddress());
				//}

			//}
		}
	
		

		ID3D11Buffer* buffers[2] = { vertexBuffer.Get(),boneDataBuffer.Get() };
		const UINT stride[2] = { vertexBuffer.Stride(),boneDataBuffer.Stride() };
		UINT offsets[2] = { 0,0 };
		this->deviceContext->IASetVertexBuffers(0, 2, buffers, stride, offsets);


		if (indexBuffer.Get())
		{
			this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

			//if(isInstanced)
			//	this->deviceContext->DrawIndexedInstanced(this->indexBuffer.IndexCount(), 1, 0, 0, 0);
			//else
				this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
		}
		else
		{
			this->deviceContext->Draw(vertexBuffer.Stride(), 0);
		}
	}
	
	
}

void Mesh::DrawUntextured()
{
	if (!deleted)
	{
		UINT offset = 0;

	

		ID3D11Buffer* buffers[2] = { vertexBuffer.Get(),boneDataBuffer.Get() };
		const UINT stride[2] = { vertexBuffer.Stride(),boneDataBuffer.Stride() };
		UINT offsets[2] = { 0,0 };
		this->deviceContext->IASetVertexBuffers(0, 2, buffers, stride, offsets);


		if (indexBuffer.Get())
		{
			this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
			this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
		}
		else
		{
			this->deviceContext->Draw(vertexBuffer.Stride(), 0);
		}
	}

}

const DirectX::XMMATRIX& Mesh::GetTranformMatrix()
{
	return this->transformMatrix;
}

void Mesh::UpdateVertices(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::vector<Vertex>& vertices)
{
	this->vertexBuffer.Initialize(device.Get(), vertices.data(), vertices.size());
}

void Mesh::DrawCube(bool isTextured, bool isPBR)
{
	UINT offset = 0;

	if (isTextured)
	{
		for (std::vector<Texture>::iterator tex = this->textures.begin(); tex != textures.end(); ++tex)
		{
			if (tex->GetType() == aiTextureType::aiTextureType_DIFFUSE)
			{
				this->deviceContext->PSSetShaderResources(0, 1, tex->GetTextureResourceViewAddress());
				//break;
			}
			//if (!blenderModel)
			//{
			//	if (tex->GetType() == aiTextureType::aiTextureType_NORMALS)
			//	{
			//		this->deviceContext->PSSetShaderResources(1, 1, tex->GetTextureResourceViewAddress());
			//	}
			//}
			//else
			//{
				if (tex->GetType() == aiTextureType::aiTextureType_HEIGHT)
				{
					this->deviceContext->PSSetShaderResources(1, 1, tex->GetTextureResourceViewAddress());
				}
			//}
			//if (tex->GetType() == aiTextureType::aiTextureType_SPECULAR)
			//{
			//	this->deviceContext->PSSetShaderResources(2, 1, tex->GetTextureResourceViewAddress());
			//}

			//if (isPBR)
			//{
			if (tex->GetType() == aiTextureType::aiTextureType_SHININESS)
			{
				this->deviceContext->PSSetShaderResources(2, 1, tex->GetTextureResourceViewAddress());
			}
			if (tex->GetType() == aiTextureType::aiTextureType_EMISSIVE)
			{
				this->deviceContext->PSSetShaderResources(3, 1, tex->GetTextureResourceViewAddress());
			}
			//}

		}
	}

	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);

	//this->deviceContext->IASetVertexBuffers(0, 1, &buffers[0], &stride[0], &offsets[0]);
	//this->deviceContext->IASetVertexBuffers(1, 1, &buffers[1], &stride[1], &offsets[1]);
	//this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	//
	//offset = sizeof(Vertex);
	//this->deviceContext->IASetVertexBuffers(1, 1, this->boneDataBuffer.GetAddressOf(), this->boneDataBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	//this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
	this->deviceContext->Draw(36, 0);
}

void Mesh::DrawRect(bool isTextured, bool isPBR)
{
	UINT offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
}

void Mesh::DrawShape(UINT& vertexCount, bool isTextured, bool isPBR)
{
	UINT offset = 0;

	if (isTextured)
	{
		for (std::vector<Texture>::iterator tex = this->textures.begin(); tex != textures.end(); ++tex)
		{
			if (tex->GetType() == aiTextureType::aiTextureType_DIFFUSE)
			{
				this->deviceContext->PSSetShaderResources(0, 1, tex->GetTextureResourceViewAddress());
				
			}
			
			if (tex->GetType() == aiTextureType::aiTextureType_NORMALS)
			{
				this->deviceContext->PSSetShaderResources(1, 1, tex->GetTextureResourceViewAddress());
			}
		
			if (tex->GetType() == aiTextureType::aiTextureType_SHININESS)
			{
				this->deviceContext->PSSetShaderResources(2, 1, tex->GetTextureResourceViewAddress());
			}
			if (tex->GetType() == aiTextureType::aiTextureType_EMISSIVE)
			{
				this->deviceContext->PSSetShaderResources(3, 1, tex->GetTextureResourceViewAddress());
			}
			

		}
	}


	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.StridePtr(), &offset);

	//this->deviceContext->IASetVertexBuffers(0, 1, &buffers[0], &stride[0], &offsets[0]);
	//this->deviceContext->IASetVertexBuffers(1, 1, &buffers[1], &stride[1], &offsets[1]);
	//this->deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	//
	//offset = sizeof(Vertex);
	//this->deviceContext->IASetVertexBuffers(1, 1, this->boneDataBuffer.GetAddressOf(), this->boneDataBuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);

	//this->deviceContext->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
	this->deviceContext->Draw(vertexCount, 0);
}
