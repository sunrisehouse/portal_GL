#include "BlockVerticesBinder.h"

BindedVertexInfo* BlockVerticesBinder::bind()
{
	GLuint vertex_array = 0;
	GLuint vertex_buffer = 0;
	GLuint index_buffer = 0;
	uint index_buffer_size = 0;

	uint number_of_triangles_in_circle = 64;

	std::vector<vertex> vertices;
	vertices.push_back({ vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f).normalize(), vec2(1.0f, 1.0f) });
	vertices.push_back({ vec3(1.0f, -1.0f, 1.0f), vec3(1.0f, -1.0f, 1.0f).normalize(), vec2(1.0f, 0.0f) });
	vertices.push_back({ vec3(-1.0f, -1.0f, 1.0f), vec3(-1.0f, -1.0f, 1.0f).normalize(), vec2(0.0f, 0.0f) });
	vertices.push_back({ vec3(-1.0f, 1.0f, 1.0f), vec3(-1.0f, 1.0f, 1.0f).normalize(), vec2(0.0f, 1.0f) });

	vertices.push_back({ vec3(1.0f, 1.0f, -1.0f), vec3(1.0f, 1.0f, -1.0f).normalize(), vec2(1.0f, 1.0f) });
	vertices.push_back({ vec3(1.0f, -1.0f, -1.0f), vec3(1.0f, -1.0f, -1.0f).normalize(), vec2(1.0f, 0.0f) });
	vertices.push_back({ vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, -1.0f, -1.0f).normalize(), vec2(0.0f, 0.0f) });
	vertices.push_back({ vec3(-1.0f, 1.0f, -1.0f), vec3(-1.0f, 1.0f, -1.0f).normalize(), vec2(0.0f, 1.0f) });

	std::vector<uint> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2); indices.push_back(0); indices.push_back(2); indices.push_back(3);
	indices.push_back(0); indices.push_back(5); indices.push_back(1); indices.push_back(0); indices.push_back(4); indices.push_back(5);
	indices.push_back(0); indices.push_back(3); indices.push_back(7); indices.push_back(0); indices.push_back(7); indices.push_back(4);
	indices.push_back(6); indices.push_back(7); indices.push_back(3); indices.push_back(6); indices.push_back(3); indices.push_back(2);
	indices.push_back(6); indices.push_back(2); indices.push_back(1); indices.push_back(6); indices.push_back(1); indices.push_back(5);
	indices.push_back(6); indices.push_back(4); indices.push_back(7); indices.push_back(6); indices.push_back(5); indices.push_back(4);

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
