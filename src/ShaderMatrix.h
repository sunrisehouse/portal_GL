#ifndef __SHADER_H__
#define __SHADER_H__

#include "cgmath.h"
#include "Camera.h"
#include "DragInfo.h"

class ShaderMatrix
{
private:
	mat4	view_matrix;
	mat4	projection_matrix;
	mat4 calculate_view_matrix(Camera camera);
	mat4 calculate_projection_matrix(ivec2 window_size, Camera camera);

public:
	ShaderMatrix(ivec2 window_size, Camera camera);
	~ShaderMatrix();

	void change_view_matrix(Camera camera);
	void change_projection_matrix(ivec2 window_size, Camera camera);

	mat4 get_view_matrix();
	mat4 get_projection_matrix();
};

#endif

