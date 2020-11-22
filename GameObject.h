#pragma once
#include <DirectXMath.h>
#include <string>
#include "OBJLoader.h"
#include "Camera.h"
#include "Structures.h"

using namespace DirectX;
using namespace std;

class GameObject {

public:
	void Init(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, ID3D11Device* _pd3dDevice, char* path);
	void Update();
	void Draw(Camera* cam, ID3D11DeviceContext* _pImmediateContext, ID3D11Buffer* _pConstantBuffer,
		ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader, ID3D11SamplerState* _pSamplerLinear);

	HRESULT AddTexture(const wchar_t* colourTexture, const wchar_t* specTexture, HRESULT hr, ID3D11Device* _pd3dDevice);

	void SetPosition(float x, float y, float z, float worldX, float worldY, float worldZ);
	void SetRotation(float x, float y, float z, float worldX, float worldY, float worldZ);
	void SetScale(float x, float y, float z);

	XMFLOAT4X4 GetTransform();

private:
	MeshData objMeshData;
	XMMATRIX transform;
	XMFLOAT3 mWorldPosition;
	XMFLOAT3 mWorldRotation;
	XMFLOAT3 mWorldScale;
	XMFLOAT3 mPosition;
	XMFLOAT3 mRotation;
	XMFLOAT3 mScale;

	XMFLOAT4X4 objMatrices;
	XMMATRIX matrix;

	int frameCount;

	

	ID3D11ShaderResourceView* _pDiffuseTexture[6];
	ID3D11ShaderResourceView* _pSpecularTexture[6];

	XMFLOAT3 lightDirection;
	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;

	XMFLOAT4 specularLight;
	XMFLOAT4 specularMaterial;
	float specularPower;
	XMVECTOR eye;
};