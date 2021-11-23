#include "GameAimingObject.h"

GameAimingObject::GameAimingObject(vec3 location, vec3 up, float theta, vec3 scale, float speed, float alpha): GameObject(location, up, theta, scale, speed), alpha(alpha)
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
