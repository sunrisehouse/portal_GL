#include "SolarSystemRotationalObject.h"

SolarSystemRotationalObject::SolarSystemRotationalObject(vec3 location, float radius, float rotational_speed): rotational_speed(rotational_speed), rotational_theta(0), radius(radius), SolarSystemObejct(location)
{
}

void SolarSystemRotationalObject::rotate(float moving_time)
{
	this->rotational_theta += moving_time * this->rotational_speed;
}

float SolarSystemRotationalObject::get_rotational_theta()
{
	return this->rotational_theta;
}

float SolarSystemRotationalObject::get_radius()
{
	return this->radius;
}

