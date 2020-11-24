#include "Player.h"

vector3d Player::GetPlayerPosition()
{
	return playerPos;
}

vector3d Player::GetPlayerRotation()
{
	return playerRot;
}

void Player::MovePlayer(float _x, float _y, float _z)
{
	playerPos.x += _x;
	playerPos.y += _y;
	playerPos.z += _z;
}

void Player::RotatePlayer(float _x, float _y, float _z)
{
	playerRot.x += _x;
	playerRot.y += _y;
	playerRot.z += _z;
}
