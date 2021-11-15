#include "SolarSystemOrbitalObject.h"

SolarSystemOrbitalObject::SolarSystemOrbitalObject(vec3 location, float radius, float rotational_speed, float orbital_speed, SolarSystemObejct* orbital_target)
	: orbital_speed(orbital_speed), orbital_target(orbital_target), SolarSystemRotationalObject(location, radius, rotational_speed)
{
	
}

void SolarSystemOrbitalObject::orbit(float moving_time)
{
	if (this->orbital_target != nullptr)
	{
		vec3 orbital_axis = vec3(0.0f, 0.0f, 1.0f);
		vec4 location = vec4(this->location, 0.0f);
		vec3 target_location = this->orbital_target->get_location();
		vec4 new_location =
			mat4::translate(target_location)
			* mat4::rotate(orbital_axis, moving_time * this->orbital_speed)
			* mat4::translate(vec3(-target_location.x, -target_location.y, -target_location.z)) * location;

		this->location = vec3(new_location.x, new_location.y, new_location.z);
	}
}
