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
	void SetPosition(const XMFLOAT3& val);

	//Get Camera basis vectors
	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	XMVECTOR GetLookXM() const;
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
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up = XMFLOAT3(0.0f, 1.0f, 0.0f));

	//Get View/Proj matrices
	XMMATRIX View() const;
	XMMATRIX Proj() const;
	XMMATRIX ViewProj() const;

	//Strafe and Walk
	void Strafe(float d);
	void Walk(float d);

	void Fly(float d);

	//Rotate the camera
	void Pitch(float angle);
	void RotateY(float angle);

	//After modifying any camera aspects you need to update the view matrix
	void UpdateViewMatrix();

private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;

	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
};