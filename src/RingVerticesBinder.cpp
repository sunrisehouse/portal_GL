#include "RingVerticesBinder.h"

BindedVertexInfo* RingVerticesBinder::bind()
{
	GLuint vertex_array = 0;
	GLuint vertex_buffer = 0;
	GLuint index_buffer = 0;
	uint index_buffer_size = 0;

	uint number_of_triangles_in_circle = 64;

	std::vector<vertex> vertices;
	for (uint k = 0; k <= number_of_triangles_in_circle; k++)
	{
		float t = PI * 2.0f * k / float(number_of_triangles_in_circle), c = cos(t), s = sin(t);
		vertices.push_back({ vec3(c,s,0), vec3(c,s,0.0f), vec2(0.0f, 0.5f) });
	}

	float inner_radius = 0.7f;
	for (uint k = 0; k <= number_of_triangles_in_circle; k++)
	{
		float t = PI * 2.0f * k / float(number_of_triangles_in_circle), c = cos(t), s = sin(t);
		vertices.push_back({ vec3(c * inner_radius,s * inner_radius,0), vec3(c,s,0.0f), vec2(1.0f, 0.5f) });
	}

	std::vector<uint> indices;
	for (uint k = 0; k < number_of_triangles_in_circle; k++)
	{
		indices.push_back(k);
		indices.push_back(k+1);
		indices.push_back(number_of_triangles_in_circle + 1 + k);
		
		indices.push_back(k + 1);
		indices.push_back(number_of_triangles_in_circle + 1 + k + 1);
		indices.push_back(number_of_triangles_in_circle + 1 + k);

		indices.push_back(k + 1);
		indices.push_back(k);
		indices.push_back(number_of_triangles_in_circle + 1 + k);

		indices.push_back(number_of_triangles_in_circle + 1 + k + 1);
		indices.push_back(k + 1);
		indices.push_back(number_of_triangles_in_circle + 1 + k);
	}

	index_buffer_size = indices.size();

	// generation of vertex buffer: use vertices as it is
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// geneation of index buffer
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); }

	return new BindedVertexInfo(vertex_buffer, index_buffer, vertex_array, index_buffer_size);
}
