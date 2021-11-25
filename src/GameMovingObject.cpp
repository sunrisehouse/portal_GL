#include "GameMovingObject.h"

GameMovingObject::GameMovingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, float moving_speed, vec3 moving_direction)
    : GameObject(location, up, theta, scale, type), moving_speed(moving_speed), moving_direction(moving_direction)
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
    this->location = this->location + this->moving_direction * this->moving_speed * moving_time;
}

float GameMovingObject::get_moving_speed()
{
    return this->moving_speed;
}

void GameMovingObject::set_moving_speed(float moving_speed)
{
    this->moving_speed = moving_speed;
}

