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
#include "GameObject.h"
#include "SphereRenderer.h"
#include "BlockRenderer.h"
#include "BlockVerticesBinder.h"

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
GameObject* sphere;
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
	sphere = new GameObject({ 30.0f, 30.0f, 30.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 10.0f, 10.0f, 10.0f });
	GameObject* plain = new GameObject({ -50.0f, -50.0f, -50.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 10.0f, 10.0f, 10.0f });

	Material* material = new Material(
		vec4(0.2f, 0.2f, 0.2f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		1000.0f
	);

	BindedVertexInfo* orb_vertex_info = OrbVerticesBinder::bind();
	BindedVertexInfo* block_bertex_info = BlockVerticesBinder::bind();

	BindedTextureInfo* earth_texture_info = TextureBinder::bind("textures/earth.jpg");

	SphereRenderer* sphereRenderer = new SphereRenderer(orb_vertex_info, earth_texture_info, sphere, material);
	BlockRenderer* blockRenderer = new BlockRenderer(block_bertex_info, earth_texture_info, plain, material);

	renderers.push_back(sphereRenderer);
	renderers.push_back(blockRenderer);
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
