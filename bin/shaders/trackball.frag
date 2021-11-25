#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

// texture sampler
uniform samplerCube cubemap;
uniform sampler2D TEX;
uniform sampler2D TEX_ALPHA;
uniform sampler2D TEX_BUMP;
uniform sampler2D TEX_NORM;
uniform int mode;

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

void main()
{
	// light position in the eye space
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	vec4 iKd = texture( TEX, tc );	// Kd from image
	if(mode==0)			fragColor = iKd;
	else if(mode==1)	
	{
	iKd = texture(TEX, tc);
	fragColor = phong( l, n, h, iKd );
	}
	else if(mode==2)
	{
		vec4 alpha = texture( TEX_ALPHA, tc );
		fragColor = vec4(phong( l, n, h, iKd ).xyz, alpha);
	}
	else
	{
		vec3 c1 = cross(n,vec3(0,0,1));
		vec3 c2 = cross(n,vec3(0,1,0));
		vec3 tangent = normalize(length(c1)>length(c2)?c1:c2);
		vec3 binormal = cross(n,tangent);

		vec3 tnormal = texture(TEX_NORM, tc ).xyz;
		tnormal = normalize(tnormal-0.5);
		mat3 tbn = mat3( tangent, binormal, n );
		vec3 world_space_bumped_normal = tbn * tnormal;

		fragColor = phong( l, world_space_bumped_normal, h, iKd );
	}
}
