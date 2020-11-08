#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>
#include "Vector3D.h"

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoords;

	bool operator<(const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
};

struct SimpleVertex1
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;

};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float specularPower;
	XMFLOAT3 EyePos;
	XMFLOAT3 LightVec3;

};