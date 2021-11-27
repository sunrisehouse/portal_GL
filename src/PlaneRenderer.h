#ifndef __PLANE_RENDERER_H__
#define __PLANE_RENDERER_H__

#include "Renderer.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "GameObject.h"
#include "cgmath.h"
#include "cgut.h"
#include "Material.h"

class PlaneRenderer : public Renderer
{
private:
    BindedVertexInfo* vertex_info;
    BindedTextureInfo* texture_info;
    GameObject* object;
    Material* material;

public:
    PlaneRenderer(
        BindedVertexInfo* vertex_info,
        BindedTextureInfo* texture_info,
        GameObject* object,
        Material* material
    );
    void render(GLuint program) override;
};

#endif
