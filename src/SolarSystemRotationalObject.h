#ifndef __SOLAR_SYSTEM_ROTATIONAL_OBJECT_H__
#define __SOLAR_SYSTEM_ROTATIONAL_OBJECT_H__

#include "SolarSystemObejct.h"

class SolarSystemRotationalObject: public SolarSystemObejct
{
protected:
	float rotational_speed;
	float rotational_theta;
	float radius;

public:
	SolarSystemRotationalObject(vec3 location, float radius, float rotational_speed);
	void rotate(float moving_time);
	float get_rotational_theta();
	float get_radius();
};

#endif
