#ifndef __RING_ORB_RENDERER__
#define __RING_ORB_RENDERER__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "SolarSystemRotationalObject.h"
#include "Material.h"
#include "cgmath.h"
#include "cgut.h"

class RingOrbRenderer: public Renderer
{
private:
	BindedVertexInfo* vertex_info;
	BindedVertexInfo* ring_vertex_info;
	BindedTextureInfo* texture_info;
	BindedTextureInfo* ring_texture_info;
	BindedTextureInfo* ring_alpha_texture_info;
	SolarSystemRotationalObject* object;
	Material* material;
	GLint mode;

public:
	RingOrbRenderer(
		BindedVertexInfo* vertex_info,
		BindedTextureInfo* texture_info,
		BindedVertexInfo* ring_vertex_info,
		BindedTextureInfo* ring_texture_info,
		BindedTextureInfo* ring_alpha_texture_info,
		SolarSystemRotationalObject* object,
		Material* material,
		GLint mode
	);
	mat4 get_model_matrix();
	mat4 get_ring_model_matrix();
	void render(GLuint program) override;
};

#endif
