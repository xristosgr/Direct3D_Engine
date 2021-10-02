#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <DirectXMath.h>
#include "Vertex.h"
#include "ErrorLogger.h"

template <class T>
class InstanceBuffer
{


private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT InstanceCount = 0;
	UINT stride = sizeof(T);

public:
	InstanceBuffer() {}
	InstanceBuffer(const InstanceBuffer<T>& rhs)
	{
		this->buffer = rhs.buffer;
		this->InstanceCount = rhs.InstanceCount;
		this->stride = rhs.stride;
	}
	InstanceBuffer<T>& operator=(const InstanceBuffer<T>& a)
	{
		this->buffer = a.buffer;
		this->InstanceCount = a.InstanceCount;
		this->stride = a.stride;
		return *this;
	}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT InstanceCount() const
	{
		return this->InstanceCount;
	}
	const UINT Stride() const
	{
		return this->stride;
	}

	const UINT* StridePtr() const
	{
		return &this->stride;
	}

	HRESULT Initialize(ID3D11Device* device, T* data, UINT InstanceCount)
	{
		if (buffer.Get() != nullptr)
			buffer.Reset();

		this->InstanceCount = InstanceCount;
		//Load Index Data
		D3D11_BUFFER_DESC InstanceBufferDesc;
		ZeroMemory(&InstanceBufferDesc, sizeof(InstanceBufferDesc));
		InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		InstanceBufferDesc.ByteWidth = stride * InstanceCount;
		InstanceBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		InstanceBufferDesc.CPUAccessFlags = 0;
		InstanceBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InstanceBufferData;
		InstanceBufferData.pSysMem = data;
		HRESULT hr = device->CreateBuffer(&InstanceBufferDesc, &InstanceBufferData, buffer.GetAddressOf());
		return hr;
	}
};
