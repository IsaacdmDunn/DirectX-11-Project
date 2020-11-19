#include "Camera.h"

Camera::Camera()
	: mPosition(0.0f,0.0f,0.0f),
	mRight(1.0f,0.0f,0.0f),
	mUp(0.0,1.0f,0.0f),
	mLook(0.0f,0.0f,1.0f)
{
	SetLens(0.25f * 3.1452, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{

}

DirectX::XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = { x,y,z };
}

void Camera::SetPosition(const XMFLOAT3& val)
{
	mPosition = val;
}

DirectX::XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&mRight);
}

DirectX::XMFLOAT3 Camera::GetRight() const
{
	return mRight;
}

DirectX::XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

DirectX::XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

DirectX::XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

DirectX::XMFLOAT3 Camera::GetLook() const
{
	return mLook;
}

float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetAspect() const
{
	return mAspect;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float Camera::GetNearWindowWidth() const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspect = aspect;

	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMMATRIX projection = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);

	XMStoreFloat4x4(&mProj, projection);
}

/// <summary>
/// Looks at a certain point in space
/// </summary>
/// <param name="pos">Position of the camera</param>
/// <param name="target">Position of the target</param>
/// <param name="worldUp">Up vector</param>
void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR l = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR r = XMVector3Normalize(XMVector3Cross(worldUp, l));
	XMVECTOR u = XMVector3Cross(l, r);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, l);
	XMStoreFloat3(&mRight, r);
	XMStoreFloat3(&mUp, u);
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR t = XMLoadFloat3(&target);
	XMVECTOR u = XMLoadFloat3(&up);

	LookAt(p, t, u);
}

DirectX::XMMATRIX Camera::View() const
{
	return XMLoadFloat4x4(&mView);
}

DirectX::XMMATRIX Camera::Proj() const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMMATRIX Camera::ViewProj() const
{
	return XMMatrixMultiply(View(), Proj());
}

void Camera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR i = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, i, p));
}

void Camera::Pitch(float angle)
{
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), r));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), r));
}

void Camera::RotateY(float angle)
{
	XMMATRIX r = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), r));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), r));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), r));
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR u = XMLoadFloat3(&mUp);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);

	l = XMVector3Normalize(l);

	u = XMVector3Normalize(XMVector3Cross(l, r));

	r = XMVector3Cross(u, l);

	float x = -XMVectorGetX(XMVector3Dot(p, r));
	float y = -XMVectorGetX(XMVector3Dot(p, u));
	float z = -XMVectorGetX(XMVector3Dot(p, l));


	XMStoreFloat3(&mRight, r);
	XMStoreFloat3(&mUp, u);
	XMStoreFloat3(&mLook, l);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;

}

