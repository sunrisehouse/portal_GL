#ifndef __GAME_MOVING_OBJECT_H__
#define __GAME_MOVING_OBJECT_H__

#include "GameObject.h"

class GameMovingObject : public GameObject
{
private:
	vec3 moving_direction;
	float moving_speed;

public:
	GameMovingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, float moving_speed, vec3 moving_direction);
	vec3 get_direction();
	void set_direction(vec3 moving_direction);
	void move(float moving_time);
	float get_moving_speed();
	void set_moving_speed(float moving_speed);
};

#endif
