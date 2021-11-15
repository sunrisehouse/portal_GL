#include "BumpedOrbRenderer.h"

BumpedOrbRenderer::BumpedOrbRenderer(
	BindedVertexInfo* vertex_info,
	BindedTextureInfo* texture_info,
	BindedTextureInfo* bump_texture_info,
	BindedTextureInfo* normal_texture_info,
	SolarSystemRotationalObject* object,
	Material* material
)
	:vertex_info(vertex_info),
	texture_info(texture_info),
	object(object),
	bump_texture_info(bump_texture_info),
	normal_texture_info(normal_texture_info),
	material(material)
{
}

mat4 BumpedOrbRenderer::get_model_matrix()
{
	vec3 location = this->object->get_location();
	float theta = this->object->get_rotational_theta();
	float radius = this->object->get_radius();

	return mat4::translate(location) *
		mat4::rotate(vec3(0, 0, 1), theta) *
		mat4::scale(radius);
}

void BumpedOrbRenderer::render(GLuint program)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_TRUE, this->get_model_matrix());

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, this->material->get_ambient());
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, this->material->get_diffuse());
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, this->material->get_specular());
	glUniform1f(glGetUniformLocation(program, "shininess"), this->material->get_shininess());

	// setup texture
	glActiveTexture(GL_TEXTURE0);								// select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, this->texture_info->get_texture());
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);	 // GL_TEXTURE0

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, this->bump_texture_info->get_texture());
	//glUniform1i(glGetUniformLocation(program, "TEX_BUMP"), 2);	 // GL_TEXTURE0
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, this->normal_texture_info->get_texture());
	glUniform1i(glGetUniformLocation(program, "TEX_NORM"), 2);	 // GL_TEXTURE0

	glUniform1i(glGetUniformLocation(program, "mode"), 3);

	// bind vertex array object
	if (this->vertex_info->get_vertex_array()) glBindVertexArray(this->vertex_info->get_vertex_array());

	glDrawElements(GL_TRIANGLES, this->vertex_info->get_index_buffer_size(), GL_UNSIGNED_INT, nullptr);
}
