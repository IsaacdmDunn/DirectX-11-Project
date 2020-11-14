#include "Camera.h"

Camera::Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
{
	_eye = XMVectorSet(position.x, position.y, position.z, 0.0f);
	_at = XMVectorSet(at.x, at.y, at.z, 0.0f);
	_up = XMVectorSet(up.x, up.y, up.z, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(_eye, _at, _up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (FLOAT)windowHeight, nearDepth, farDepth));
}

Camera::~Camera()
{
	
}

void Camera::Update(float x, float y, float z)
{
	_eye += XMVectorSet(x, y, z, 0.0f);
	_at += XMVectorSet(x,y,z, 0.0f);
	_up = XMVectorSet(0,1,0, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(_eye, _at, _up));

	// Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (FLOAT)windowHeight, nearDepth, farDepth));

}

void Camera::InitCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT
	windowWidth, FLOAT windowHeight, FLOAT
	nearDepth, FLOAT farDepth)
{
	_eye = XMVectorSet(position.x, position.y, position.z, 0.0f);
	_at = XMVectorSet(at.x, at.y, at.z, 0.0f);
	_up = XMVectorSet(up.x, up.y, up.z, 0.0f);

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(_eye, _at, _up));

	// Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, windowWidth / (FLOAT)windowHeight, nearDepth, farDepth));

}

void Camera::MoveCameraTo(int x, int y, int z)
{
}

void Camera::MoveCamera(int x, int y, int z)
{
}

vector3d Camera::GetPos()
{
	return vector3d();
}

vector3d Camera::GetAt()
{
	return vector3d();
}

vector3d Camera::GetUp()
{
	return vector3d();
}

XMFLOAT4X4 Camera::getViewMatrix()
{
	return _view;
}

XMFLOAT4X4 Camera::getProjectionMatrix()
{
	return _projection;
}
