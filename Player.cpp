#include "Player.h"

//gets player position
vector3d Player::GetPlayerPosition()
{
	return playerPos;
}

//gets player rotation
vector3d Player::GetPlayerRotation()
{
	return playerRot;
}

//moves player
void Player::MovePlayer(float _x, float _y, float _z)
{
	playerPos.x += _x;
	playerPos.y += _y;
	playerPos.z += _z;
}

//rotates player
void Player::RotatePlayer(float _x, float _y, float _z)
{
	playerRot.x += _x;
	playerRot.y += _y;
	playerRot.z += _z;
}
