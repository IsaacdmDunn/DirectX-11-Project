#include "GameObject.h"
#include "DDSTextureLoader.h"

//inialise game object
void GameObject::Init(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale, ID3D11Device* _pd3dDevice, char* path)
{
	_position = pos;
	_rotation = rot;
	_scale = scale;
	_objMeshData = OBJLoader::Load(path, _pd3dDevice, false);
	
	_matrix = XMMatrixIdentity();
	XMStoreFloat4x4(&_objMatrices, _matrix);
}

//update 
void GameObject::Update()
{
	_matrix = XMMatrixIdentity();
	_matrix = XMMatrixMultiply(_matrix, XMMatrixScaling(_scale.x, _scale.y, _scale.z));
	_matrix = XMMatrixMultiply(_matrix, XMMatrixTranslation(_position.x, _position.y, _position.z));
	_matrix = XMMatrixMultiply(_matrix, XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z));
	_matrix = XMMatrixMultiply(_matrix, XMMatrixTranslation(_worldPosition.x, _worldPosition.y, _worldPosition.z));
	_matrix = XMMatrixMultiply(_matrix, XMMatrixRotationRollPitchYaw(_worldRotation.x, _worldRotation.y, _worldRotation.z));
	XMStoreFloat4x4(&_objMatrices, _matrix);
}

//draw
void GameObject::Draw(Camera* cam, ID3D11DeviceContext* _pImmediateContext, ID3D11Buffer* _pConstantBuffer, 
	ID3D11VertexShader* _pVertexShader, ID3D11PixelShader* _pPixelShader, ID3D11SamplerState* _pSamplerLinear)
{
	//set lights up
	lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
	diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ambientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.2f);
	ambientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	specularLight = XMFLOAT4(0.7f, 0.0f, 0.9f, 1.0f);
	specularMaterial = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);
	specularPower = 1.1f;
	eye = cam->GetPositionXM();

	//sets up mesh to buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_objMeshData.VertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_objMeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	XMMATRIX world = XMLoadFloat4x4(&_objMatrices);

	//sets constant buffer
	ConstantBuffer cb;
	cb._view = XMMatrixTranspose(cam->View());
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

	//draws game object with shaders applied
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pDiffuseTexture[1]);
	_pImmediateContext->PSSetShaderResources(1, 1, &_pSpecularTexture[1]);
	_pImmediateContext->DrawIndexed(_objMeshData.IndexCount, 0, 0);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

}

//added texture to game object from file
HRESULT GameObject::AddTexture(const wchar_t* colourTexture, const wchar_t* specTexture, HRESULT hr, ID3D11Device* _pd3dDevice)
{
	hr = CreateDDSTextureFromFile(_pd3dDevice, specTexture, nullptr, &_pSpecularTexture[1]);
	hr = CreateDDSTextureFromFile(_pd3dDevice, colourTexture, nullptr, &_pDiffuseTexture[1]);

	return hr;
}

//sets position of game object
void GameObject::SetPosition(float x, float y, float z, float worldX, float worldY, float worldZ)
{
	_position = { x, y, z };
	_worldPosition = { worldX, worldY, worldZ };
}

//sets rotation of game object
void GameObject::SetRotation(float x, float y, float z, float worldX, float worldY, float worldZ)
{
	_rotation = { x, y, z };
	_worldRotation = {worldX, worldY, worldZ};
}

//sets scale of game object
void GameObject::SetScale(float x, float y, float z)
{
	_scale = { x, y, z };
}

//gets the transform of the game object
XMFLOAT4X4 GameObject::GetTransform()
{
	return _objMatrices;
}
