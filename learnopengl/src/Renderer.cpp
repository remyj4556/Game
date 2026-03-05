#include "../include/Renderer.hpp"
#include "../include/ChunkCoordinates.hpp"
#include "../include/Chunk.hpp"
#include "../include/Camera.hpp"
#include "../include/LightManager.hpp"
#include "../include/Mesh.hpp"
#include "../include/TextureAtlas.hpp"


Renderer::Renderer(GLFWwindow* window) :
	// TODO: look into shader buffer or something else other than hardcoding them in here
	block_shader("C:/Users/remyj/source/repos/Game/learnopengl/shaders/lightingShader.vs", "C:/Users/remyj/source/repos/Game/learnopengl/shaders/lightingShader.fs"),
	light_shader("C:/Users/remyj/source/repos/Game/learnopengl/shaders/lightCubeShader.vs", "C:/Users/remyj/source/repos/Game/learnopengl/shaders/lightCubeShader.fs")
{
	glfwGetWindowSize(window, &screen_width, &screen_height);

	glViewport(0, 0, screen_width, screen_height);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

void Renderer::beginFrame(Camera& camera, LightManager& light_manager, const TextureAtlas *block_atlas) {
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear the depth buffer before each render iteration specifically

	// view/projection matrix transformations
	projection = glm::perspective(glm::radians(camera.fov), (float)screen_width / (float)screen_height, 0.1f, render_distance);
	view = camera.getViewMatrix();

	block_shader.use();

	// set camera
	block_shader.setMat4("view", view);
	block_shader.setMat4("projection", projection);
	block_shader.setVec3("view_pos", camera.position);
	
	// set sun lighting
	const DirectionalLight& sun = light_manager.getSun();
	block_shader.setVec3("sun.direction", sun.direction);
	block_shader.setVec3("sun.ambient", sun.ambient);
	block_shader.setVec3("sun.diffuse", sun.diffuse);
	block_shader.setVec3("sun.specular", sun.specular);
	block_shader.setBool("sun.enabled", sun.enabled);

	// TODO: set moon lighting

	// set player light if enabled
	const PointLight& player_light = light_manager.getPlayerLight();
	block_shader.setBool("player_light.enabled", player_light.enabled);
	if (player_light.enabled) {
		block_shader.setVec3("player_light.position", player_light.position);
		block_shader.setVec3("player_light.color", player_light.color);
		block_shader.setFloat("player_light.intensity", player_light.intensity);
		block_shader.setFloat("player_light.radius", player_light.radius);
	}

	// bind texture atlas once per frame
	block_atlas->atlas->bind();
}

void Renderer::renderChunk(Chunk& chunk) {
	// translate chunk model matrix based on position in world
	glm::mat4 model = glm::mat4(1.0f);

	// chunk position uses relative coordinates (e.g., chunk at (1,1,1) is actually at block position (32, 32, 32))
	Coordinates chunk_pos = chunk.chunk_position; 
	int chunk_size = chunk.getChunkSize();

	chunk_pos.x *= chunk_size;
	chunk_pos.y *= chunk_size;
	chunk_pos.z *= chunk_size;

	model = glm::translate(model, glm::vec3(chunk_pos.x, chunk_pos.y, chunk_pos.z));
	block_shader.setMat4("model", model);

	// draw chunk
	chunk.chunk_mesh.draw();
}

// temporarily take in light mesh, idk how to render more than one light rn anyways
void Renderer::renderDebugLight(Mesh& light_mesh, const glm::vec3& light_pos) {
	// use shader
	light_shader.use();

	// set uniform(s) for view and projection transformations (reuse same ones for cube)
	light_shader.setMat4("view", view);
	light_shader.setMat4("projection", projection);

	// world transformation(s)
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, light_pos);
	model = glm::scale(model, glm::vec3(0.2f));
	light_shader.setMat4("model", model);

	// set color (the color of the cube itself) of light 
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
	light_shader.setVec3("color", light_color);

	// render the cube
	light_mesh.draw();
}

void Renderer::endFrame() {

}

