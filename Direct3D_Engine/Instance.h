#pragma once
#include <DirectXMath.h>

class Instance
{
	Instance(float x, float y, float z)
		: pos(x, y, z)
	{

	}


	DirectX::XMFLOAT3 GetPos()
	{
		return pos;
	}

	DirectX::XMFLOAT3 pos;
};

