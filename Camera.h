#pragma once
#include "Structures.h"

class Camera
{
private:
	// Private attributes to store the camera position and view 
	// volume

	XMVECTOR _eye;
	XMVECTOR _at;
	XMVECTOR _up;

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	// attributes to hold the view and projection matrices which 
	// will be passed to the shader

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:

	//Constructor and destructor for the camera

	Camera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT
		windowWidth, FLOAT windowHeight, FLOAT
		nearDepth, FLOAT farDepth);
	~Camera();

	// update function to make the current view and projection 
// matrices
	void Update();
	void InitCamera(XMFLOAT3 position, XMFLOAT3 at, XMFLOAT3 up, FLOAT
		windowWidth, FLOAT windowHeight, FLOAT
		nearDepth, FLOAT farDepth);
	void MoveCameraTo(int x, int y, int z);
	void MoveCamera(int x, int y, int z);

	// you will need a set of functions to set and return the 
// position, lookat and up attributes
	vector3d GetPos();
	vector3d GetAt();
	vector3d GetUp();
		// You will need functions to get the View, Projection and 
	// combined ViewProjection matrices

		// A function to reshape the camera volume if the window is 
	// resized.
		void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT
			nearDepth, FLOAT farDepth);
		XMFLOAT4X4 getViewMatrix();
		XMFLOAT4X4 getProjectionMatrix();
};

