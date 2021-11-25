#ifndef __GAME_AIMING_OBJECT_H__
#define __GAME_AIMING_OBJECT_H__

#include "GameMovingObject.h"

class GameAimingObject: public GameMovingObject
{
private:
	float alpha;
public:
	GameAimingObject(vec3 location, vec3 up, float theta, vec3 scale, int type, float moving_speed, vec3 moving_direction, float alpha);
	float get_alpha();
	void set_alpha(float alpha);
};

#endif
