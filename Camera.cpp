#include "Camera.h"

Camera::Camera()
{
	_position = { 0.0f, 0.0f, 0.0f }; 
	_right = { 1.0f, 0.0f, 0.0f };
	_up = { 0.0f, 1.0f, 0.0f };
	_look = { 0.0f, 0.0f, 1.0f };
	SetLens(3.14/4, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

//gets position from xmfloat
DirectX::XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&_position);
}

//gets position vector
DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return _position;
}

//set position with vector
void Camera::SetPosition(float x, float y, float z)
{
	_position = { x,y,z };
}

//sets position from xmfloat
void Camera::SetPosition(const XMFLOAT3& val)
{
	_position = val;
}

//get right vector
DirectX::XMFLOAT3 Camera::GetRight() const
{
	return _right;
}

//get up vector
DirectX::XMFLOAT3 Camera::GetUp() const
{
	return _up;
}

//get look vector
DirectX::XMFLOAT3 Camera::GetLook() const
{
	return _look;
}

//get near clipping 
float Camera::GetNearZ() const
{
	return _nearZ;
}

//get far clipping
float Camera::GetFarZ() const
{
	return _farZ;
}

//get aspect ratio
float Camera::GetAspect() const
{
	return _aspect;
}

//get feild of view y
float Camera::GetFovY() const
{
	return _fovY;
}

//get field of view x
float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / _nearZ);
}

//get near window height
float Camera::GetNearWindowHeight() const
{
	return _nearWindowHeight;
}

//get near window width
float Camera::GetNearWindowWidth() const
{
	return _aspect * _nearWindowHeight;
}

//get far window height
float Camera::GetFarWindowHeight() const
{
	return _farWindowHeight;
}

//get far window width
float Camera::GetFarWindowWidth() const
{
	return _aspect * _farWindowHeight;
}

//set up the camera's values
void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	_fovY = fovY;
	_aspect = aspect;

	_nearZ = zn;
	_farZ = zf;

	_nearWindowHeight = 2.0f * _nearZ * tanf(0.5f * _fovY);
	_farWindowHeight = 2.0f * _farZ * tanf(0.5f * _fovY);

	XMMATRIX projection = XMMatrixPerspectiveFovLH(_fovY, _aspect, _nearZ, _farZ);

	XMStoreFloat4x4(&_projection, projection);
}

//looks at a position in world space for camera to point at
void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR look = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	XMVECTOR up = XMVector3Cross(look, right);

	XMStoreFloat3(&_position, pos);
	XMStoreFloat3(&_look, look);
	XMStoreFloat3(&_right, right);
	XMStoreFloat3(&_up, up);
}

//looks at a position in world space for camera to point at
void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR _pos = XMLoadFloat3(&pos);
	XMVECTOR _target = XMLoadFloat3(&target);
	XMVECTOR _up = XMLoadFloat3(&up);

	LookAt(_pos, _target, _up);
}


//gets view
DirectX::XMMATRIX Camera::View() const
{
	return XMLoadFloat4x4(&_view);
}

//gets projection
DirectX::XMMATRIX Camera::Proj() const
{
	return XMLoadFloat4x4(&_projection);
}

//strafe from side to side
void Camera::Strafe(float distance)
{
	XMVECTOR dist = XMVectorReplicate(distance);
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(dist, right, pos));
}

//walk forward and back
void Camera::Walk(float distance)
{
	XMVECTOR dist = XMVectorReplicate(distance);
	XMVECTOR look = XMLoadFloat3(&_look);
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(dist, look, pos));
}

//fly up and down
void Camera::Fly(float distance)
{
	XMVECTOR dist = XMVectorReplicate(distance);
	XMVECTOR look = XMLoadFloat3(&_up);
	XMVECTOR pos = XMLoadFloat3(&_position);
	XMStoreFloat3(&_position, XMVectorMultiplyAdd(dist, look, pos));
}

//pitch camera
void Camera::Pitch(float angle)
{
	XMMATRIX right = XMMatrixRotationAxis(XMLoadFloat3(&_right), angle);

	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), right));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), right));
}

//rotate camera on y axis
void Camera::RotateY(float angle)
{
	XMMATRIX right = XMMatrixRotationY(angle);

	XMStoreFloat3(&_right, XMVector3TransformNormal(XMLoadFloat3(&_right), right));
	XMStoreFloat3(&_up, XMVector3TransformNormal(XMLoadFloat3(&_up), right));
	XMStoreFloat3(&_look, XMVector3TransformNormal(XMLoadFloat3(&_look), right));
}

//update view matrix for the camera
void Camera::UpdateViewMatrix()
{
	XMVECTOR right = XMLoadFloat3(&_right);
	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR look = XMLoadFloat3(&_look);
	XMVECTOR pos = XMLoadFloat3(&_position);

	look = XMVector3Normalize(look);
	up = XMVector3Normalize(XMVector3Cross(look, right));
	right = XMVector3Cross(up, look);

	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	XMStoreFloat3(&_right, right);
	XMStoreFloat3(&_up, up);
	XMStoreFloat3(&_look, look);

	_view(0, 0) = _right.x;
	_view(1, 0) = _right.y;
	_view(2, 0) = _right.z;
	_view(3, 0) = x;
	_view(0, 1) = _up.x;
	_view(1, 1) = _up.y;
	_view(2, 1) = _up.z;
	_view(3, 1) = y;
	_view(0, 2) = _look.x;
	_view(1, 2) = _look.y;
	_view(2, 2) = _look.z;
	_view(3, 2) = z;
	_view(0, 3) = 0.0f;
	_view(1, 3) = 0.0f;
	_view(2, 3) = 0.0f;
	_view(3, 3) = 1.0f;

}

