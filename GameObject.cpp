#include "GameObject.h"
#include "DDSTextureLoader.h"

void GameObject::Init(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, ID3D11Device* _pd3dDevice, char* path)
{
	mPosition = pos;
	mRotation = rot;
	mScale = scale;
	objMeshData = OBJLoader::Load(path, _pd3dDevice, false);
	
	matrix = XMMatrixIdentity();
	XMStoreFloat4x4(&objMatrices, matrix);
}

void GameObject::Update()
{
	matrix = XMMatrixIdentity();
	matrix = XMMatrixMultiply(matrix, XMMatrixScaling(mScale.x, mScale.y, mScale.z));
	matrix = XMMatrixMultiply(matrix, XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z));
	matrix = XMMatrixMultiply(matrix, XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z));
	matrix = XMMatrixMultiply(matrix, XMMatrixTranslation(mWorldPosition.x, mWorldPosition.y, mWorldPosition.z));
	matrix = XMMatrixMultiply(matrix, XMMatrixRotationRollPitchYaw(mWorldRotation.x, mWorldRotation.y, mWorldRotation.z));
	XMStoreFloat4x4(&objMatrices, matrix);
}

void GameObject::Draw(Camera* cam, ID3D11DeviceContext* _pImmediateContext, ID3D11Buffer* _pConstantBuffer, 
	ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader, ID3D11SamplerState* _pSamplerLinear)
{
	lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
	diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	ambientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	specularLight = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);
	specularMaterial = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);
	specularPower = 1;
	eye = cam->GetPositionXM();

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	_pImmediateContext->IASetVertexBuffers(0, 1, &objMeshData.VertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	XMMATRIX world = XMLoadFloat4x4(&objMatrices);
	ConstantBuffer cb;
	cb.mView = XMMatrixTranspose(cam->View());
	cb.mProjection = XMMatrixTranspose(cam->Proj());
	cb.mWorld = XMMatrixTranspose(world);

	cb.DiffuseMtrl = diffuseMaterial;
	cb.DiffuseLight = diffuseLight;
	cb.LightVec3 = lightDirection;

	cb.AmbientLight = ambientLight;
	cb.AmbientMtrl = ambientMaterial;

	cb.SpecularLight = specularLight;
	cb.SpecularMtrl = specularMaterial;
	cb.specularPower = specularPower;
	cb.EyePos = { eye.m128_f32[0], eye.m128_f32[1], eye.m128_f32[2] };

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[1]);
	_pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[1]);
	_pImmediateContext->DrawIndexed(objMeshData.IndexCount, 0, 0);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
}

HRESULT GameObject::AddTexture(const wchar_t* colourTexture, const wchar_t* specTexture, HRESULT hr, ID3D11Device* _pd3dDevice)
{
	hr = CreateDDSTextureFromFile(_pd3dDevice, specTexture, nullptr, &_pSpecularTexture[1]);
	hr = CreateDDSTextureFromFile(_pd3dDevice, colourTexture, nullptr, &_pDiffuseTexture[1]);

	return hr;
}

void GameObject::SetPosition(float x, float y, float z, float worldX, float worldY, float worldZ)
{
	mPosition = { x, y, z };
	mWorldPosition = { worldX, worldY, worldZ };
}

void GameObject::SetRotation(float x, float y, float z, float worldX, float worldY, float worldZ)
{
	mRotation = { x, y, z };
	mWorldRotation = {worldX, worldY, worldZ};
}

void GameObject::SetScale(float x, float y, float z)
{
	mScale = { x, y, z };
}

XMFLOAT4X4 GameObject::GetTransform()
{
	return objMatrices;
}
