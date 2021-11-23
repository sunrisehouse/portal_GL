#include "GameObject.h"

GameObject::GameObject(vec3 location, vec3 up, float theta, vec3 scale, float speed): location(location), up(up), theta(theta), scale(scale), speed(speed)
{
}

vec3 GameObject::get_location()
{
	return this->location;
}

void GameObject::set_location(vec3 location)
{
	this->location = location;
}

vec3 GameObject::get_up()
{
	return this->up;
}

void GameObject::set_up(vec3 up)
{
	this->up = up;
}

float GameObject::get_theta()
{
	return this->theta;
}

vec3 GameObject::get_forward()
{
	vec4 forward = mat4::rotate(this->up, this->theta) * vec4(0.0f, 1.0f, 0.0f, 0.0f);
	
	return vec3(forward.x, forward.y, forward.z);
}

void GameObject::set_theta(float theta)
{
	this->theta = theta;
}

vec3 GameObject::get_scale()
{
	return this->scale;
}

void GameObject::set_scale(vec3 scale)
{
	this->scale = scale;
}

float GameObject::get_speed()
{
	return this->speed;
}

void GameObject::set_speed(float speed)
{
	this->speed = speed;
}