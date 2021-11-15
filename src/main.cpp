#include "cgmath.h"		// slee's simple math library
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "cgut.h"		// slee's OpenGL utility
#include "Camera.h"
#include "ShaderMatrix.h"
#include "DragInfo.h"
#include "Light.h"
#include "Material.h"
#include "SolarSystemRotationalObject.h"
#include "SolarSystemOrbitalObject.h"
#include "BindedVertexInfo.h"
#include "OrbVerticesBinder.h"
#include "RingVerticesBinder.h"
#include "BindedTextureInfo.h"
#include "TextureBinder.h"
#include "Renderer.h"
#include "OrbRenderer.h"
#include "RingOrbRenderer.h"
#include "BumpedOrbRenderer.h"

//*************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "shaders/trackball.vert";
static const char*	frag_shader_path = "shaders/trackball.frag";

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size;

//*************************************
// OpenGL objects
GLuint	program	= 0;	// ID holder for GPU program

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
float prev_time;

//*************************************
// scene objects
Camera*		camera = nullptr;
Light* light = nullptr;
ShaderMatrix* shader_matrix = nullptr;
DragInfo* drag_info = nullptr;
bool is_left_mouse_pressed = false;
bool is_left_shift_pressed = false;
bool is_left_ctrl_pressed = false;
bool is_polygon_mode = false;

//*************************************

std::vector<Renderer*> renderers;
std::vector<SolarSystemRotationalObject*> rotatinal_objects;
std::vector<SolarSystemOrbitalObject*> orbital_objects;

//*************************************

vec2 cursor_to_ndc(dvec2 cursor, ivec2 window_size)
{
	// normalize window pos to [0,1]^2
	vec2 npos = vec2(float(cursor.x) / float(window_size.x - 1),
		float(cursor.y) / float(window_size.y - 1));

	// normalize window pos to [-1,1]^2 with vertical flipping
	// vertical flipping: window coordinate system defines y from
	// top to bottom, while the trackball from bottom to top
	return vec2(npos.x * 2.0f - 1.0f, 1.0f - npos.y * 2.0f);
}

void update()
{
	float current_time = float(glfwGetTime()) * 0.4f;

	float moving_time = current_time - prev_time;
	prev_time = current_time;

	for (auto& object : rotatinal_objects)
	{
		object->rotate(moving_time);
	}

	for (auto& object : orbital_objects)
	{
		object->orbit(moving_time);
	}
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_TRUE, shader_matrix->get_view_matrix());
	glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_TRUE, shader_matrix->get_projection_matrix());

	// setup light properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light->get_position());
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light->get_ambient());
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light->get_diffuse());
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light->get_specular());

	for (auto& renderer : renderers)
	{
		renderer->render(program);
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
	shader_matrix->change_projection_matrix(window_size, *camera);
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press Home to reset camera\n" );
	printf("- press 'w' to toggle wireframe\n");
	printf( "\n" );
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)	glfwSetWindowShouldClose(window, GL_TRUE);
		else if (key == GLFW_KEY_H || key == GLFW_KEY_F1)	print_help();
		else if (key == GLFW_KEY_HOME)					camera->initialize();
		else if (key == GLFW_KEY_LEFT_SHIFT) is_left_shift_pressed = true;
		else if (key == GLFW_KEY_LEFT_CONTROL) is_left_ctrl_pressed = true;
		else if (key == GLFW_KEY_W)
		{
			is_polygon_mode = !is_polygon_mode;
			glPolygonMode(GL_FRONT_AND_BACK, is_polygon_mode ? GL_LINE : GL_FILL);
			if (is_polygon_mode)
			{
				printf("> using wireframe mode");
			}
			else
			{
				printf("> using solid mode");
			}
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_SHIFT) is_left_shift_pressed = false;
		else if (key == GLFW_KEY_LEFT_CONTROL) is_left_ctrl_pressed = false;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			is_left_mouse_pressed = true;
			dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
			vec2 npos = cursor_to_ndc(pos, window_size);
			drag_info->start(npos);
		}
		else if (action == GLFW_RELEASE)
		{
			is_left_mouse_pressed = false;
		}
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if (is_left_mouse_pressed)
	{
		vec2 current_position = cursor_to_ndc(dvec2(x, y), window_size);
		drag_info->change_position(current_position);

		if (is_left_shift_pressed)
		{
			vec2 prev_position = drag_info->get_prev_position();
			camera->zoom(current_position.y - prev_position.y);
			shader_matrix->change_view_matrix(*camera);
			shader_matrix->change_projection_matrix(window_size, *camera);
		}
		else if (is_left_ctrl_pressed)
		{
			vec2 prev_position = drag_info->get_prev_position();
			camera->pan(current_position - prev_position);
			shader_matrix->change_view_matrix(*camera);
			shader_matrix->change_projection_matrix(window_size, *camera);
		}
		else
		{
			vec2 prev_position = drag_info->get_prev_position();
			camera->track_ball(prev_position, current_position);
			shader_matrix->change_view_matrix(*camera);
			shader_matrix->change_projection_matrix(window_size, *camera);
		}
	}
}

void user_init()
{
	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void user_finalize()
{
}

void create_solar_system()
{

	SolarSystemRotationalObject* sun = new SolarSystemRotationalObject({ 0.0f, 0.0f, 0.0f }, 30.0f, 1.2f);
	SolarSystemOrbitalObject* mercury = new SolarSystemOrbitalObject({ 60.0f, 0.0f, 0.0f }, 4.0f, 3.0f, 1.5f, sun);
	SolarSystemOrbitalObject* venus = new SolarSystemOrbitalObject({ 80.0f, 0.0f, 0.0f }, 5.0f, 3.0f, 1.9f, sun);
	SolarSystemOrbitalObject* earth = new SolarSystemOrbitalObject({ 100.0f, 0.0f, 0.0f }, 6.0f, 3.0f, -1.8f, sun);
	SolarSystemOrbitalObject* mars = new SolarSystemOrbitalObject({ 120.0f, 0.0f, 0.0f }, 7.0f, 3.0f, 1.6f, sun);
	SolarSystemOrbitalObject* jupiter = new SolarSystemOrbitalObject({ 150.0f, 0.0f, 0.0f }, 10.0f, 3.0f, -1.8f, sun);
	SolarSystemOrbitalObject* saturn = new SolarSystemOrbitalObject({ 200.0f, 0.0f, 0.0f }, 11.0f, 3.0f, 0.9f, sun);
	SolarSystemOrbitalObject* uranus = new SolarSystemOrbitalObject({ 250.0f, 0.0f, 0.0f }, 12.0f, 3.0f, -1.1f, sun);
	SolarSystemOrbitalObject* neptune = new SolarSystemOrbitalObject({ 290.0f, 0.0f, 0.0f }, 5.0f, 3.0f, 1.4f, sun);

	rotatinal_objects.push_back(sun);
	rotatinal_objects.push_back(mercury);
	rotatinal_objects.push_back(venus);
	rotatinal_objects.push_back(earth);
	rotatinal_objects.push_back(mars);
	rotatinal_objects.push_back(jupiter);
	rotatinal_objects.push_back(saturn);
	rotatinal_objects.push_back(uranus);
	rotatinal_objects.push_back(neptune);

	orbital_objects.push_back(mercury);
	orbital_objects.push_back(venus);
	orbital_objects.push_back(earth);
	orbital_objects.push_back(mars);
	orbital_objects.push_back(jupiter);
	orbital_objects.push_back(saturn);
	orbital_objects.push_back(uranus);
	orbital_objects.push_back(neptune);

	Material* material = new Material(
		vec4(0.2f, 0.2f, 0.2f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		1000.0f
	);

	BindedVertexInfo* orb_vertex_info = OrbVerticesBinder::bind();
	BindedVertexInfo* ring_vertex_info = RingVerticesBinder::bind();
	BindedTextureInfo* sun_texture_info = TextureBinder::bind("textures/sun.jpg");
	BindedTextureInfo* mercury_texture_info = TextureBinder::bind("textures/mercury.jpg");
	BindedTextureInfo* mercury_bump_texture_info = TextureBinder::bind("textures/mercury-bump.jpg");
	BindedTextureInfo* mercury_normal_texture_info = TextureBinder::bind("textures/mercury-normal.jpg");
	BindedTextureInfo* venus_texture_info = TextureBinder::bind("textures/venus.jpg");
	BindedTextureInfo* venus_bump_texture_info = TextureBinder::bind("textures/venus-bump.jpg");
	BindedTextureInfo* venus_normal_texture_info = TextureBinder::bind("textures/venus-normal.jpg");
	BindedTextureInfo* earth_texture_info = TextureBinder::bind("textures/earth.jpg");
	BindedTextureInfo* earth_bump_texture_info = TextureBinder::bind("textures/earth-bump.jpg");
	BindedTextureInfo* earth_normal_texture_info = TextureBinder::bind("textures/earth-normal.jpg");
	BindedTextureInfo* mars_texture_info = TextureBinder::bind("textures/mars.jpg");
	BindedTextureInfo* mars_bump_texture_info = TextureBinder::bind("textures/mars=bump.jpg");
	BindedTextureInfo* mars_normal_texture_info = TextureBinder::bind("textures/mars-normal.jpg");
	BindedTextureInfo* jupiter_texture_info = TextureBinder::bind("textures/jupiter.jpg");
	BindedTextureInfo* saturn_texture_info = TextureBinder::bind("textures/saturn.jpg");
	BindedTextureInfo* saturn_ring_texture_info = TextureBinder::bind("textures/saturn-ring.jpg");
	BindedTextureInfo* saturn_ring_alpha_texture_info = TextureBinder::bind("textures/saturn-ring-alpha.jpg");
	BindedTextureInfo* uranus_texture_info = TextureBinder::bind("textures/uranus.jpg");
	BindedTextureInfo* uranus_ring_texture_info = TextureBinder::bind("textures/uranus-ring.jpg");
	BindedTextureInfo* uranus_ring_alpha_texture_info = TextureBinder::bind("textures/uranus-ring-alpha.jpg");
	BindedTextureInfo* neptune_texture_info = TextureBinder::bind("textures/neptune.jpg");

	OrbRenderer* sun_renderers = new OrbRenderer(orb_vertex_info, sun_texture_info, sun, material, 0);
	BumpedOrbRenderer* mercury_renderers = new BumpedOrbRenderer(orb_vertex_info, mercury_texture_info, mercury_bump_texture_info, mercury_normal_texture_info, mercury, material);
	BumpedOrbRenderer* venus_renderers = new BumpedOrbRenderer(orb_vertex_info, venus_texture_info, venus_bump_texture_info, venus_normal_texture_info, venus, material);
	BumpedOrbRenderer* earth_renderers = new BumpedOrbRenderer(orb_vertex_info, earth_texture_info, earth_bump_texture_info, earth_normal_texture_info, earth, material);
	BumpedOrbRenderer* mars_renderers = new BumpedOrbRenderer(orb_vertex_info, mars_texture_info, mars_bump_texture_info, mars_normal_texture_info, mars, material);
	OrbRenderer* jupiter_renderers = new OrbRenderer(orb_vertex_info, jupiter_texture_info, jupiter, material, 1);
	RingOrbRenderer* saturn_renderers = new RingOrbRenderer(orb_vertex_info, saturn_texture_info, ring_vertex_info, saturn_ring_texture_info, saturn_ring_alpha_texture_info, saturn, material, 1);
	RingOrbRenderer* uranus_renderers = new RingOrbRenderer(orb_vertex_info, uranus_texture_info, ring_vertex_info, uranus_ring_texture_info, uranus_ring_alpha_texture_info, uranus, material, 1);
	OrbRenderer* neptune_renderers = new OrbRenderer(orb_vertex_info, neptune_texture_info, neptune, material, 1);
	
	renderers.push_back(sun_renderers);
	renderers.push_back(mercury_renderers);
	renderers.push_back(venus_renderers);
	renderers.push_back(earth_renderers);
	renderers.push_back(mars_renderers);
	renderers.push_back(jupiter_renderers);
	renderers.push_back(saturn_renderers);
	renderers.push_back(uranus_renderers);
	renderers.push_back(neptune_renderers);
}

int main( int argc, char* argv[] )
{
	window_size = cg_default_window_size(); // initial window size
	camera = new Camera();
	shader_matrix = new ShaderMatrix(window_size, *camera);
	drag_info = new DragInfo();
	light = new Light(
		vec4(0.0f, 0.0f, 0.0f, 1.0f),
		vec4(0.2f, 0.2f, 0.2f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f)
	);

	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	user_init();					// user initialization

	create_solar_system();

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	print_help();

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
