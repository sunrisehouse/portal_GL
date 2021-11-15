#ifndef __SOLAR_SYSTEM_OBJECT_H__
#define __SOLAR_SYSTEM_OBJECT_H__

#include "cgmath.h"
#include "cgut.h"

class SolarSystemObejct
{
protected:
	vec3 location;

public:
	SolarSystemObejct(vec3 location);
	vec3 get_location();
};

#endif
