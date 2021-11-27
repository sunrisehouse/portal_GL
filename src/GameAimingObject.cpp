#include "GameAimingObject.h"

GameAimingObject::GameAimingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, vec3 velocity, float alpha)
	: GameMovingObject(location, up, theta, scale, type, velocity), alpha(alpha)
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
