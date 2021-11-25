#include "GameMovingObject.h"

GameMovingObject::GameMovingObject(vec3 location, vec3 up, float theta, vec3 scale, float speed, vec3 moving_direction)
    : GameObject(location, up, theta, scale, speed), moving_direction(moving_direction)
{

}

vec3 GameMovingObject::get_direction()
{
    return this->moving_direction;
}

void GameMovingObject::set_direction(vec3 moving_direction)
{
    this->moving_direction = moving_direction.normalize();
}

void GameMovingObject::move(float moving_time)
{
    this->location = this->location + this->moving_direction * this->speed * moving_time;
}
