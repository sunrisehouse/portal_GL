#ifndef __SOLAR_SYSTEM_ORBITAL_OBJECT_H__
#define __SOLAR_SYSTEM_ORBITAL_OBJECT_H__

#include "SolarSystemObejct.h"
#include "SolarSystemRotationalObject.h"

class SolarSystemOrbitalObject: public SolarSystemRotationalObject
{
protected:
	float	orbital_speed;
	SolarSystemObejct* orbital_target;

public:
	SolarSystemOrbitalObject(vec3 location, float radius, float rotational_speed, float orbital_speed, SolarSystemObejct* orbital_target);
	void orbit(float moving_time);
};

#endif
