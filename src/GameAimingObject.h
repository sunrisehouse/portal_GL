#ifndef __GAME_AIMING_OBJECT_H__
#define __GAME_AIMING_OBJECT_H__

#include "GameObject.h"

class GameAimingObject: public GameObject
{
private:
	float alpha;
public:
	GameAimingObject(vec3 location, vec3 up, float theta, vec3 scale, float speed, float alpha);
	float get_alpha();
	void set_alpha(float alpha);
};

#endif
