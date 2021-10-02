#include "TerrainCellClass.h"

TerrainCellClass::TerrainCellClass()
{
	m_vertexList = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_lineVertexBuffer = 0;
	m_lineIndexBuffer = 0;
}

bool TerrainCellClass::Initialize(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY,int cellHeight, int cellWidth, int terrainWidth)
{
	ModelType* terrainModel;
	bool result;


	// Coerce the pointer to the terrain model into the model type.
	terrainModel = (ModelType*)terrainModelPtr;

	// Load the rendering buffers with the terrain data for this cell index.
	result = InitializeBuffers(device, nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, terrainModel);
	if (!result)
	{
		return false;
	}

	// Release the pointer to the terrain model now that we no longer need it.
	terrainModel = 0;

	// Calculuate the dimensions of this cell.
	CalculateCellDimensions();

	// Build the debug line buffers to produce the bounding box around this cell.
	result = BuildLineBuffers(device);
	if (!result)
	{
		return false;
	}

	return true;
}

void TerrainCellClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int TerrainCellClass::GetVertexCount()
{
	return m_vertexCount;
}

int TerrainCellClass::GetIndexCount()
{
	return m_indexCount;
}

int TerrainCellClass::GetLineBuffersIndexCount()
{
	return 0;
}

void TerrainCellClass::GetCellDimensions(float&, float&, float&, float&, float&, float&)
{
}

bool TerrainCellClass::InitializeBuffers(ID3D11Device* device, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth,int terrainWidth, ModelType* terrainModel)
{
	int i, j, modelIndex, index;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = (cellHeight - 1) * (cellWidth - 1) * 6;
	m_indexCount = m_vertexCount;

	vertices.resize(m_vertexCount);
	vertex2.resize(m_vertexCount);
	m_indexCount = m_vertexCount;
	indices.resize(m_indexCount);

	

	modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	index = 0;
	for (j = 0; j < cellHeight-1; j++)
	{
		for (i = 0; i < (cellWidth - 1)*6; i++)
		{
			vertices[index].pos = DirectX::XMFLOAT3(terrainModel[modelIndex].x, terrainModel[modelIndex].y, terrainModel[modelIndex].z);
			vertices[index].texCoord = DirectX::XMFLOAT2(terrainModel[modelIndex].tu, terrainModel[modelIndex].tv);
			vertices[index].normal = DirectX::XMFLOAT3(terrainModel[modelIndex].nx, terrainModel[modelIndex].ny, terrainModel[modelIndex].nz);
			vertices[index].tangent = DirectX::XMFLOAT3(terrainModel[modelIndex].tx, terrainModel[modelIndex].ty, terrainModel[modelIndex].tz);
			vertices[index].binormal = DirectX::XMFLOAT3(terrainModel[modelIndex].bx, terrainModel[modelIndex].by, terrainModel[modelIndex].bz);

			//vertex2[index].color = DirectX::XMFLOAT3(terrainModel[modelIndex].r, terrainModel[modelIndex].g, terrainModel[modelIndex].b);
			vertex2[index].texCoord2 = DirectX::XMFLOAT2(terrainModel[modelIndex].tu2, terrainModel[modelIndex].tv2);

			indices[index] = index;



			//GENERATE TANGENTS
			//########################################################################
			//########################################################################
						// Generate tangents
			//DirectX::XMVECTOR normalVec = XMLoadFloat3(&vertices[index].normal);
			//
			//DirectX::XMFLOAT3 tempFloat3 = DirectX::XMFLOAT3(0.0f, 0.0f, 0.1f);
			//DirectX::XMVECTOR tempVec = XMLoadFloat3(&tempFloat3);
			//DirectX::XMVECTOR t1 = DirectX::XMVector3Cross(normalVec, tempVec);
			//
			//tempFloat3 = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			//tempVec = XMLoadFloat3(&tempFloat3);
			//DirectX::XMVECTOR t2 = DirectX::XMVector3Cross(normalVec, tempVec);
			//
			//DirectX::XMVECTOR t1LengthVec = DirectX::XMVector3Length(t1);
			//DirectX::XMFLOAT3 t1LengthFloat3;
			//XMStoreFloat3(&t1LengthFloat3, t1LengthVec);
			//
			//DirectX::XMVECTOR t2LengthVec = DirectX::XMVector3Length(t2);
			//DirectX::XMFLOAT3 t2LengthFloat3;
			//XMStoreFloat3(&t2LengthFloat3, t2LengthVec);
			//
			//if (t1LengthFloat3.x > t2LengthFloat3.x && t1LengthFloat3.y > t2LengthFloat3.y && t1LengthFloat3.z > t2LengthFloat3.z)
			//{
			//	XMStoreFloat3(&vertices[index].tangent, t1);
			//}
			//else
			//{
			//	XMStoreFloat3(&vertices[index].tangent, t2);
			//}
			////#################################################################################
			////#################################################################################
			//
			//
			////GENERATE BINORMALS
			////########################################################################
			////########################################################################
			//// generate binormals(bitangents)
			//DirectX::XMVECTOR tangent = XMLoadFloat3(&vertices[index].tangent);
			//DirectX::XMVector3Normalize(tangent);
			//DirectX::XMVECTOR binormal = XMLoadFloat3(&vertices[index].binormal);
			//DirectX::XMVECTOR normal = XMLoadFloat3(&vertices[index].normal);
			//
			//binormal = DirectX::XMVector3Cross(tangent, normal);
			//
			//XMStoreFloat3(&vertices[index].binormal, binormal);
			//#################################################################################
			//#################################################################################
			modelIndex++;
			index++;
		}
		modelIndex += (terrainWidth * 6) - (cellWidth * 6);
		
	}
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{

		return false;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex2) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertex2.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	
	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer2);
	if (FAILED(result))
	{
	
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void TerrainCellClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	
	//mesh->Draw(isTextured, false);

	unsigned int stride[2];
	unsigned int offset[2];


	// Set vertex buffer stride and offset.
	ID3D11Buffer* buffers[2] = { m_vertexBuffer,m_vertexBuffer2 };
	stride[0] = sizeof(Vertex);
	stride[1] = sizeof(m_vertexBuffer2);
	offset[0] = 0;
	offset[1] = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, buffers, stride, offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(indices.size(), 0, 0);
}

void TerrainCellClass::CalculateCellDimensions()
{
	int i;
	float width, height, depth;


	// Initialize the dimensions of the node.
	m_maxWidth = -1000000.0f;
	m_maxHeight = -1000000.0f;
	m_maxDepth = -1000000.0f;

	m_minWidth = 1000000.0f;
	m_minHeight = 1000000.0f;
	m_minDepth = 1000000.0f;

	for (i = 0; i < m_vertexCount; i++)
	{
		width = vertices[i].pos.x;
		height = vertices[i].pos.y;
		depth = vertices[i].pos.z;

		// Check if the width exceeds the minimum or maximum.
		if (width > m_maxWidth)
		{
			m_maxWidth = width;
		}
		if (width < m_minWidth)
		{
			m_minWidth = width;
		}

		// Check if the height exceeds the minimum or maximum.
		if (height > m_maxHeight)
		{
			m_maxHeight = height;
		}
		if (height < m_minHeight)
		{
			m_minHeight = height;
		}

		// Check if the depth exceeds the minimum or maximum.
		if (depth > m_maxDepth)
		{
			m_maxDepth = depth;
		}
		if (depth < m_minDepth)
		{
			m_minDepth = depth;
		}
	}

	// Calculate the center pos of this cell.
	m_positionX = (m_maxWidth - m_minWidth) + m_minWidth;
	m_positionY = (m_maxHeight - m_minHeight) + m_minHeight;
	m_positionZ = (m_maxDepth - m_minDepth) + m_minDepth;

}

bool TerrainCellClass::BuildLineBuffers(ID3D11Device* device)
{
	std::vector<Vertex> vertices;
	//std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	DirectX::XMFLOAT3 lineColor;
	int index, vertexCount, indexCount;


	// Set the color of the lines to orange.
	lineColor = DirectX::XMFLOAT3(1.0f, 0.5f, 0.0f);

	// Set the number of vertices in the vertex array.
	vertexCount = 24;

	// Set the number of indices in the index array.
	indexCount = vertexCount;

	// Create the vertex array.
	vertices.resize(m_vertexCount);
	m_indexCount = m_vertexCount;
	debugIndices.resize(m_indexCount);



	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = debugIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Load the vertex and index array with data.
	index = 0;

	// 8 Horizontal lines.
	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	// 4 Verticle lines.
	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_maxHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_minHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_maxHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_minHeight, m_maxDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_maxHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_maxWidth, m_minHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_maxHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;
	index++;

	vertices[index].pos = DirectX::XMFLOAT3(m_minWidth, m_minHeight, m_minDepth);
	//vertices[index].color = lineColor;
	debugIndices[index] = index;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_lineVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_lineColorvertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_lineIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Store the index count for rendering.
	m_lineIndexCount = indexCount;

	// Release the arrays now that the vertex and index buffers have been created and loaded.


	return true;
}

void TerrainCellClass::RenderLineBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride[1];
	unsigned int offset[1];


	// Set vertex buffer stride and offset.
	ID3D11Buffer* buffers[1] = { m_lineVertexBuffer};
	stride[0] = sizeof(Vertex);
	//stride[1] = sizeof(ColorVertex);
	offset[0] = 0;
	//offset[1] = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, buffers, stride, offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->DrawIndexed(debugIndices.size(), 0, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void TerrainCellClass::DeleteCells()
{
	vertices.clear();
	indices.clear();
	debugIndices.clear();
	deleted = true;
}
