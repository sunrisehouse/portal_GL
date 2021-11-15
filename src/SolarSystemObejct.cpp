#include "SolarSystemObejct.h"

SolarSystemObejct::SolarSystemObejct(vec3 location): location(location)
{
}

vec3 SolarSystemObejct::get_location()
{
	return this->location;
}
