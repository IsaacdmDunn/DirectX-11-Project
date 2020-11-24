#pragma once

#include "GameObject.h"

class Player : public GameObject {
public:
	vector3d GetPlayerPosition();
	vector3d GetPlayerRotation();
	void MovePlayer(float _x, float _y, float _z);
	void RotatePlayer(float _x, float _y, float _z);
private:
	vector3d playerPos;
	vector3d playerRot;
};
