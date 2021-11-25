#include "cgmath.h"		// slee's simple math library
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "cgut.h"		// slee's OpenGL utility
#include "Camera.h"
#include "ViewProjectionMatrix.h"
#include "DragHistory.h"
#include "Light.h"
#include "Material.h"
#include "BindedVertexInfo.h"
#include "BindedTextureInfo.h"
#include "TextureBinder.h"
#include "Renderer.h"
#include "GameObject.h"
#include "GameAimingObject.h"
#include "GameMovingObject.h"
#include "SphereRenderer.h"
#include "BlockRenderer.h"
#include "BlockVerticesBinder.h"
#include "SphereVerticesBinder.h"
#include "MousePositionHistory.h"

//*************************************
// global constants
static const char*	window_name = "game";
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
struct {
	bool left = false,
		right = false,
		up = false,
		down = false;
	operator bool() const {
		return left || right || up || down;
	}
}mov_key;
float gravity = -500;
bool onGround = false;
float jump_power = 200;
float upward_speed = 0;
vec3 prev_loc;
std::vector<GameObject*> blocks;
//*************************************
// scene objects
Camera*		camera = nullptr;
Light* light = nullptr;
ViewProjectionMatrix* view_projection_matrix = nullptr;
MousePositionHistory* mouse_position_history = nullptr;
bool is_left_shift_pressed = false;
bool is_left_ctrl_pressed = false;
bool is_polygon_mode = false;

//*************************************

BindedVertexInfo* sphere_vertex_info;
BindedVertexInfo* block_vertex_info;
BindedTextureInfo* box_texture_info;
Material* default_material;

//*************************************

GameAimingObject* sphere;
GameMovingObject* blue_bullet;
GameMovingObject* yellow_bullet;

//*************************************

std::vector<Renderer*> renderers;

//*************************************

void change_aim_by_mouse(GameAimingObject* game_object, vec2 mouse_path)
{
	float x_threshold = 0.0001f;
	float y_threhold = 0.0001f;

	if (mouse_path.x >= x_threshold || mouse_path.x <= -x_threshold)
	{
		game_object->set_theta(game_object->get_theta() - mouse_path.x * 0.5f);
	}
	if(mouse_path.y >= y_threhold || mouse_path.y <= -y_threhold)
	{
		game_object->set_alpha(game_object->get_alpha() - mouse_path.y * 0.5f);
	}
}

void dangle_canera_to_game_object(Camera* cam, GameAimingObject* game_object)
{
	vec3 location = game_object->get_location();
	vec3 up = game_object->get_up();
	vec3 forward = game_object->get_forward();
	float alpha = game_object->get_alpha();

	vec3 camera_position_offset = vec3(0.0f, 0.0f, 10.0f);
	vec4 new_up = mat4::rotate(cross(up, forward), alpha) * vec4(up, 0.0f);
	vec4 new_at = mat4::rotate(cross(up, forward), alpha) * vec4(forward, 0.0f);

	cam->set_eye(location + camera_position_offset);
	cam->set_up(vec3(new_up.x, new_up.y, new_up.z));
	cam->set_at(location + camera_position_offset + vec3(new_at.x, new_at.y, new_at.z) * 1000.0f);
}

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

void sphere_movement(float moving_time)
{
	vec3 loc = sphere->get_location();
	vec3 forward = sphere->get_forward();
	vec3 velocity = vec3(0.0f, 0.0f, 0.0f);
	if (mov_key.up) {
		velocity = vec3(forward.x, forward.y, 0.0f).normalize() * (is_left_shift_pressed?2.0f:1.0f);
	}
	if (mov_key.down) {
		velocity = -vec3(forward.x, forward.y, 0.0f).normalize()/2;
	}
	if (mov_key.left) {
		velocity = vec3(-forward.y, forward.x, 0.0f).normalize()/2;
	}
	if (mov_key.right) {
		velocity = vec3(forward.y, -forward.x, 0.0f).normalize()/2;
	}
	sphere->set_location(loc + velocity * moving_time * 1000.0f);
}

void gravity_handler(float moving_time) {
	upward_speed += gravity * moving_time;
	if (upward_speed < -30) {
		onGround = false;
	}
	vec3 loc = sphere->get_location();
	sphere->set_location(loc + vec3(0, 0, upward_speed*moving_time));
}

void collision_handler() {
	vec3 loc = sphere->get_location();
	vec3 moving_vector = loc - prev_loc;
	moving_vector.z = 0;
	for (auto& b : blocks)
	{
		vec3 b_loc = b->get_location();
		vec3 b_scale = b->get_scale();
		if (abs(loc.x - b_loc.x) < b_scale.x/2) {
			if (abs(loc.y - b_loc.y) < b_scale.y/2) {
				if (abs(loc.z - b_loc.z) < b_scale.z) {
					if (prev_loc.z - b_loc.z > b_scale.z) {
						onGround = true;
						upward_speed = 0;
						sphere->set_z(b_loc.z + b_scale.z + 0.1f);
					}
					else {
						sphere->set_location(loc - moving_vector*1.001f);
					}
				}
			}
		}
	}
}

void jump() {
	if (onGround) {
		upward_speed = jump_power;
		onGround = false;
	}
}
void update()
{
	float current_time = float(glfwGetTime()) * 0.4f;

	float moving_time = current_time - prev_time;
	prev_time = current_time;
	prev_loc = sphere->get_location();
	sphere_movement(moving_time);
	gravity_handler(moving_time);
	for (auto& b : blocks) {
		collision_handler();
	}
	
	vec2 current_position = mouse_position_history->get_current_position();
	vec2 prev_position = mouse_position_history->get_prev_position();
	if (current_position != prev_position) {
		change_aim_by_mouse(sphere, current_position - prev_position);
		mouse_position_history->make_prev_position();
	}
	if (sphere->get_location().z < -200.0f) {
		sphere->set_location(vec3(0, 0, 10));
	}
	dangle_canera_to_game_object(camera, sphere);
	view_projection_matrix->change_view_matrix(*camera);

	if (blue_bullet)
	{
		blue_bullet->move(moving_time);
	}
	if (yellow_bullet)
	{
		yellow_bullet->move(moving_time);
	}
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_TRUE, view_projection_matrix->get_view_matrix());
	glUniformMatrix4fv(glGetUniformLocation(program, "projection_matrix"), 1, GL_TRUE, view_projection_matrix->get_projection_matrix());

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
	view_projection_matrix->change_projection_matrix(window_size, *camera);
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
		else if (key == GLFW_KEY_W)	mov_key.up = true;
		else if (key == GLFW_KEY_A)	mov_key.left = true;
		else if (key == GLFW_KEY_S)	mov_key.down = true;
		else if (key == GLFW_KEY_D)	mov_key.right = true;
		else if (key == GLFW_KEY_SPACE) jump();
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_LEFT_SHIFT) is_left_shift_pressed = false;
		else if (key == GLFW_KEY_LEFT_CONTROL) is_left_ctrl_pressed = false;
		else if (key == GLFW_KEY_W)	mov_key.up = false;
		else if (key == GLFW_KEY_A)	mov_key.left = false;
		else if (key == GLFW_KEY_S)	mov_key.down = false;
		else if (key == GLFW_KEY_D)	mov_key.right = false;
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && blue_bullet == nullptr)
	{
		vec3 location = camera->get_eye();
		vec3 moving_direction = camera->get_at();
		blue_bullet = new GameMovingObject(location, { 0.0f, 0.0f, 1.0f }, 0.0f, { 5.0f, 5.0f, 5.0f }, 0, moving_direction * 0.001f);
		renderers.push_back(new SphereRenderer(sphere_vertex_info, box_texture_info, blue_bullet, default_material));
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && yellow_bullet == nullptr)
	{
		vec3 location = camera->get_eye();
		vec3 moving_direction = camera->get_at();
		yellow_bullet = new GameMovingObject(location, { 0.0f, 0.0f, 1.0f }, 0.0f, { 5.0f, 5.0f, 5.0f }, 0, moving_direction * 0.001f);
		renderers.push_back(new SphereRenderer(sphere_vertex_info, box_texture_info, yellow_bullet, default_material));
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	vec2 current_position = cursor_to_ndc(dvec2(x, y), window_size);
	mouse_position_history->change_position(current_position);
}

void user_init()
{
	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_BLEND);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void user_finalize()
{
}

void create_graphic_object()
{
	sphere_vertex_info = SphereVerticesBinder::bind();
	block_vertex_info = BlockVerticesBinder::bind();

	box_texture_info = TextureBinder::bind("textures/box.png");
	default_material = new Material(
		vec4(0.5f, 0.5f, 0.5f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		1000.0f
	);
}

void create_map()
{
	GameObject* plain = new GameObject({ 0.0f, 0.0f, -10.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 1000.0f, 1000.0f, 10.0f }, 0);
	GameObject* box1 = new GameObject({ 200.0f, 100.0f, 10.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 150.0f, 20.0f }, 0);
	GameObject* box2 = new GameObject({ -200.0f, -100.0f, 10.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 100.0f, 150.0f, 200.0f }, 0);
	blocks.push_back(plain);
	blocks.push_back(box1);
	blocks.push_back(box2);
}

void create_game_object()
{
	sphere = new GameAimingObject({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 1.0f }, 0.0f, { 5.0f, 5.0f, 5.0f }, 0, vec3(0.0f, 0.0f, 0.0f), 0.0f);

	SphereRenderer* sphereRenderer = new SphereRenderer(sphere_vertex_info, box_texture_info, sphere, default_material);
	renderers.push_back(sphereRenderer);

	for (auto& b : blocks) {
		BlockRenderer* blockRenderer = new BlockRenderer(block_vertex_info, box_texture_info, b, default_material);
		renderers.push_back(blockRenderer);
	}
}

int main( int argc, char* argv[] )
{
	window_size = cg_default_window_size(); // initial window size
	camera = new Camera();
	view_projection_matrix = new ViewProjectionMatrix(window_size, *camera);
	mouse_position_history = new MousePositionHistory();
	light = new Light(
		vec4(0.0f, 0.0f, 100.0f, 1.0f),
		vec4(1.0f, 1.0f, 1.0f, 1.0f),
		vec4(0.8f, 0.8f, 0.8f, 1.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
	);

	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	user_init();					// user initialization

	create_graphic_object();
	create_map();
	create_game_object();

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
