#ifndef __ORB_RENDERER_H__
#define __ORB_RENDERER_H__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "SolarSystemRotationalObject.h"
#include "cgmath.h"
#include "cgut.h"
#include "Material.h"

class OrbRenderer: public Renderer
{
private:
	BindedVertexInfo* vertex_info;
	BindedTextureInfo* texture_info;
	SolarSystemRotationalObject* object;
	GLint mode;
	Material* material;

public:
	OrbRenderer(
		BindedVertexInfo* vertex_info,
		BindedTextureInfo* texture_info,
		SolarSystemRotationalObject* object,
		Material* material,
		GLint mode
	);
	mat4 get_model_matrix();
	void render(GLuint program) override;
};

#endif
