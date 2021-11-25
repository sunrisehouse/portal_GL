#include "GameAimingObject.h"

GameAimingObject::GameAimingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, float moving_speed, vec3 moving_direction, float alpha)
	: GameMovingObject(location, up, theta, scale, type, moving_speed, moving_direction), alpha(alpha)
{
}

float GameAimingObject::get_alpha()
{
	return this->alpha;
}

void GameAimingObject::set_alpha(float alpha)
{
	this->alpha = alpha;
}
