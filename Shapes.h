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

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	float gTime;
};

struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
SimpleVertex cubeVertices[] =
{
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },

	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
};

// Create vertex buffer
SimpleVertex planeVertices[] =
{
	{ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //bl
	{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) }, //br
	{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, //tl
	{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //tr
};

// Create vertex buffer
SimpleVertex pyramidVertices[] =
{
	{ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, //bl
	{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) }, //br
	{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, //tl
	{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //tr
	{ XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }, //top
};

class Shapes
{



private:
	

public:
	Shapes();
	~Shapes();

	void InitCube();
	void InitPyramid();
	void InitPlane();

	void UpdateCube();
	void UpdatePyramid();
	void UpdatePlane();
	void DrawCube(ID3D11Buffer* pCubeIndexBuffer, ID3D11DeviceContext* pImmediateContext, ID3D11VertexShader* pVertexShader, ID3D11PixelShader* pPixelShader, ID3D11Buffer* pConstantBuffer, std::vector<XMFLOAT4X4> worldMatrices, int i, XMFLOAT4X4 view, XMFLOAT4X4 projection);
	void DrawPyramid();
	void DrawPlane();


	ID3D11Buffer* _pCubeVertexBuffer;
	ID3D11Buffer* _pPlaneVertexBuffer;
	ID3D11Buffer* _pPyramidVertexBuffer;
};