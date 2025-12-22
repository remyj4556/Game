#include "../include/Renderer.hpp"

Renderer::Renderer(const unsigned int screen_width, const unsigned int screen_height) :
	screen_width(screen_width),
	screen_height(screen_height),
	block_shader("C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightingShader.vs", "C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightingShader.fs"),
	light_shader("C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightCubeShader.vs", "C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightCubeShader.fs"),
	block_atlas("textures")
{
	// create and fill block registry (don't use singleton eventually)
	auto& block_reg = BlockRegistry::getInstance();
	block_reg.populateDefinitions("json/blocks.json", block_atlas);

	glViewport(0, 0, screen_width, screen_height);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer() {}

void Renderer::beginFrame(Camera& camera, glm::vec3& light_pos) {
	glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear the depth buffer before each render iteration specifically

	// view/projection matrix transformations
	projection = glm::perspective(glm::radians(camera.fov), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
	view = camera.getViewMatrix();

	// set up lighting shader with lighting info (TODO: make lighting manager for several lights)
	block_shader.use();

	// set the light position and view position 
	block_shader.setVec3("light.position", light_pos);
	block_shader.setVec3("view_pos", camera.position);
	
	// TODO: eventually move material properties OUT of Renderer entirely
	// light properties
	glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuse_color = light_color * glm::vec3(0.5f); // decrease the influence
	glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f); // low influence

	block_shader.setVec3("light.ambient", ambient_color);
	block_shader.setVec3("light.diffuse", diffuse_color);
	block_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	block_shader.setVec3("material.ambient", 0.8f, 0.8f, 0.8f);
	block_shader.setVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
	block_shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	block_shader.setFloat("material.shininess", 32.0f);
}

void Renderer::renderChunk(Chunk& chunk) {
	// set uniform(s) for these transformations
	block_shader.setMat4("view", view);
	block_shader.setMat4("projection", projection);

	// TODO: this (translate) will change eventually based on chunk position in world
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, chunk.chunk_position);
	block_shader.setMat4("model", model);

	// draw chunk
	block_atlas.atlas->bind();
	chunk.chunk_mesh.draw();
}

// temporarily take in light mesh, idk how to render more than one light rn anyways
void Renderer::renderLight(Mesh& light_mesh, glm::vec3& light_pos) {
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

