#include "DragInfo.h"

DragInfo::DragInfo()
{
}

DragInfo::~DragInfo()
{
}

void DragInfo::start(vec2 position)
{
	this->initial_position = position;
	this->current_position = position;
}

void DragInfo::change_position(vec2 position)
{
	this->prev_position = this->current_position;
	this->current_position = position;
}

vec2 DragInfo::get_initial_position()
{
	return this->initial_position;
}

vec2 DragInfo::get_current_position()
{
	return this->current_position;
}

vec2 DragInfo::get_prev_position()
{
	return this->prev_position;
}
