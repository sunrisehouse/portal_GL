#include "ShaderMatrix.h"

mat4 ShaderMatrix::calculate_view_matrix(Camera camera)
{
	return mat4::look_at(camera.get_eye(), camera.get_at(), camera.get_up());
}

mat4 ShaderMatrix::calculate_projection_matrix(ivec2 window_size, Camera camera)
{
	float aspect = window_size.x / float(window_size.y);
	return mat4::perspective(camera.get_fovy(), aspect, camera.get_dnear(), camera.get_dfar());
}

ShaderMatrix::ShaderMatrix(ivec2 window_size, Camera camera)
{
	this->view_matrix = this->calculate_view_matrix(camera);
	this->projection_matrix = this->calculate_projection_matrix(window_size, camera);
}

ShaderMatrix::~ShaderMatrix()
{
}

void ShaderMatrix::change_view_matrix(Camera camera)
{
	this->view_matrix = this->calculate_view_matrix(camera);
}

void ShaderMatrix::change_projection_matrix(ivec2 window_size, Camera camera)
{
	this->projection_matrix = this->calculate_projection_matrix(window_size, camera);
}

mat4 ShaderMatrix::get_view_matrix()
{
	return this->view_matrix;
}

mat4 ShaderMatrix::get_projection_matrix()
{
	return this->projection_matrix;
}
