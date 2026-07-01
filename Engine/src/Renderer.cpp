#include "../include/Renderer.hpp"
#include "../include/Coordinates.hpp"
#include "../include/RenderRequest.hpp"
#include "../include/Chunk.hpp"
#include "../include/Camera.hpp"
#include "../include/LightManager.hpp"
#include "../include/Mesh.hpp"
#include "../include/TextureLibrary.hpp"
#include "../include/Paths.hpp"
#include "../include/GPUBlockDefinition.hpp"
#include "../include/glm/fwd.hpp"
#include "../include/glm/glm.hpp"
#include <queue>
#include <vector>
#include <utility>

Renderer::Renderer(GLFWwindow* window, std::queue<RenderRequest>& load_queue, std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue, const Paths& paths)
	: render_distance(500.0f)
	, block_shader(paths.shaders / "lightingShader.vs", paths.shaders / "lightingShader.fs")
	, light_shader(paths.shaders / "lightCubeShader.vs", paths.shaders / "lightCubeShader.fs")
	, clear_color(DEFAULT_COLOR)
	, load_queue(load_queue)
	, unload_queue(unload_queue)
{
	glfwGetWindowSize(window, &screen_width, &screen_height);

	glViewport(0, 0, screen_width, screen_height);
	
	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// backface culling
	glEnable(GL_CULL_FACE);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	// wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

Renderer::~Renderer() {}

void Renderer::beginFrame(Camera& camera, LightManager& light_manager, const TextureLibrary& texture_library) {
	glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// view/projection matrix transformations
	projection = glm::perspective(glm::radians(camera.fov), (float)screen_width / (float)screen_height, 0.1f, render_distance);
	view = camera.getViewMatrix();

	block_shader.use();

	// set camera
	block_shader.setMat4("view", view);
	block_shader.setMat4("projection", projection);
	block_shader.setVec3("view_pos", camera.position);

	// time of day affects lighting
	block_shader.setFloat("time_of_day", light_manager.getTimeOfDay());
	
	// set sun lighting
	const DirectionalLight& sun = light_manager.getSun();
	block_shader.setVec3("sun.direction", sun.direction);
	block_shader.setVec3("sun.ambient", sun.ambient);
	block_shader.setVec3("sun.diffuse", sun.diffuse);
	block_shader.setVec3("sun.specular", sun.specular);
	block_shader.setBool("sun.enabled", sun.enabled);

	// set player light if enabled
	const PointLight& player_light = light_manager.getPlayerLight();
	block_shader.setBool("player_light.enabled", player_light.enabled);
	if (player_light.enabled) {
		block_shader.setVec3("player_light.position", player_light.position);
		block_shader.setVec3("player_light.color", player_light.color);
		block_shader.setFloat("player_light.intensity", player_light.intensity);
		block_shader.setFloat("player_light.radius", player_light.radius);
	}

	// bind texture library once per frame
	texture_library.bind();
}

void Renderer::processQueuedChunkMeshes() {
	while (!load_queue.empty()) {
		RenderRequest current_request = std::move(load_queue.front());
		load_queue.pop();

		chunk_meshes[current_request.chunk_coord] = std::move(current_request.chunk_mesh);
	}
}

void Renderer::drawChunks() {
	int chunk_size = Chunk::CHUNK_SIZE;

	// draw all chunks in hashmap
	for (const auto& [chunk_coord, chunk_mesh] : chunk_meshes) {
		glm::mat4 model = glm::mat4(1.0f);

		int x = chunk_coord.x * chunk_size;
		int y = chunk_coord.y * chunk_size;
		int z = chunk_coord.z * chunk_size;

		// translate chunk model matrix based on position in world
		model = glm::translate(model, glm::vec3(x, y, z));
		block_shader.setMat4("model", model);

		chunk_mesh.draw();
	}
}

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

void Renderer::unloadChunkMesh(CoordinateSystem::ChunkCoordinates chunk_coord) {
	chunk_meshes.erase(chunk_coord);
}

void Renderer::uploadGPUBlockDefinitions(std::vector<GPUBlockDefinition> gpu_definitions) const {
	GLuint block_defs_ubo;
	glGenBuffers(1, &block_defs_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, block_defs_ubo);
	glBufferData(GL_UNIFORM_BUFFER, gpu_definitions.size() * sizeof(GPUBlockDefinition), gpu_definitions.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, block_defs_ubo);

	block_shader.linkUBO("BlockBuffer");
}

void Renderer::setClearColor(glm::vec4 color) {
	clear_color = color;
}

glm::vec4 Renderer::getClearColor() const {
	return clear_color;
}

glm::vec4 Renderer::getDefaultClearColor() const {
	return DEFAULT_COLOR;
}

void Renderer::setRenderDistance(float value) {
	render_distance = value;
}

float Renderer::getRenderDistance() const {
	return render_distance;
}

void Renderer::renderRendererDebugInfo() const {

}
