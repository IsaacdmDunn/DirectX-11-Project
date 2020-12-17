#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Camera {
public:
	Camera();
	~Camera();

	//Get/Set World Position
	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& pos);

	//Get Camera basis vectors
	XMFLOAT3 GetRight() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetLook() const;

	//Get frustum properties
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	//Get near and far plane dimensions in view space coordinates
	float GetNearWindowHeight() const;
	float GetNearWindowWidth() const;
	float GetFarWindowHeight() const;
	float GetFarWindowWidth() const;

	//Set frustum
	void SetLens(float fovY, float aspect, float zn, float zf);

	//Look at targets
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	//Get View/Proj matrices
	XMMATRIX View() const;
	XMMATRIX Proj() const;

	//Strafe and Walk
	void Strafe(float distance);
	void Walk(float distance);
	void Fly(float distance);

	//Rotate the camera
	void Pitch(float angle);
	void RotateY(float angle);

	//After modifying any camera aspects you need to update the view matrix
	void UpdateViewMatrix();

private:
	XMFLOAT3 _position;
	XMFLOAT3 _right;
	XMFLOAT3 _up;
	XMFLOAT3 _look;

	float _nearZ;
	float _farZ;
	float _aspect;
	float _fovY;
	float _nearWindowHeight;
	float _farWindowHeight;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;
};