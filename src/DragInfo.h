#ifndef __DRAGINFO_H__
#define __DRAGINFO_H__

#include "cgmath.h"

class DragInfo
{
private:
	vec2 initial_position;
	vec2 prev_position;
	vec2 current_position;

public:
	DragInfo();
	~DragInfo();
	void start(vec2 position);
	void change_position(vec2 position);
	vec2 get_initial_position();
	vec2 get_current_position();
	vec2 get_prev_position();
};

#endif
