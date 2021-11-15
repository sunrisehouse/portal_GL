#ifndef __BUMPED_ORB_RENDERER_H__
#define __BUMPED_ORB_RENDERER_H__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "SolarSystemRotationalObject.h"
#include "cgmath.h"
#include "cgut.h"
#include "Material.h"

class BumpedOrbRenderer : public Renderer
{
private:
	BindedVertexInfo* vertex_info;
	BindedTextureInfo* texture_info;
	BindedTextureInfo* bump_texture_info;
	BindedTextureInfo* normal_texture_info;
	SolarSystemRotationalObject* object;
	Material* material;

public:
	BumpedOrbRenderer(
		BindedVertexInfo* vertex_info,
		BindedTextureInfo* texture_info,
		BindedTextureInfo* bump_texture_info,
		BindedTextureInfo* normal_texture_info,
		SolarSystemRotationalObject* object,
		Material* material
	);
	mat4 get_model_matrix();
	void render(GLuint program) override;
};

#endif
