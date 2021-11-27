#include "PlaneRenderer.h"

PlaneRenderer::PlaneRenderer(BindedVertexInfo* vertex_info, BindedTextureInfo* texture_info, GameObject* object, Material* material)
	:vertex_info(vertex_info), texture_info(texture_info), object(object), material(material)
{
}

void PlaneRenderer::render(GLuint program)
{

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, this->material->get_ambient());
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, this->material->get_diffuse());
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, this->material->get_specular());
	glUniform1f(glGetUniformLocation(program, "shininess"), this->material->get_shininess());

	// setup texture
	glActiveTexture(GL_TEXTURE0);                        // select the texture slot to bind
	glBindTexture(GL_TEXTURE_2D, this->texture_info->get_texture());
	glUniform1i(glGetUniformLocation(program, "TEX"), 0);    // GL_TEXTURE0
	glUniform1i(glGetUniformLocation(program, "mode"), 1);

	// bind vertex array object
	if (this->vertex_info->get_vertex_array()) glBindVertexArray(this->vertex_info->get_vertex_array());

	glDrawElements(GL_TRIANGLES, this->vertex_info->get_index_buffer_size(), GL_UNSIGNED_INT, nullptr);
}
