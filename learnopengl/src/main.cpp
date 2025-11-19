#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "../include/Mesh.hpp"
#include "../include/Shader.hpp"
#include "../include/Texture.hpp"
#include "../include/Camera.hpp"
#include "../include/BlockRegistry.hpp"
#include "../include/ModelLibrary.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/Chunk.hpp"
#include "../include/TextureAtlas.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in);
void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = SCR_WIDTH / 2.0f;
float last_y = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// delta time 
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// light source position
glm::vec3 light_pos(0.0f, 5.0f, 0.0f);


int main() {
	glfwInit();
	// below two lines ensure that the correct OpenGL version is present, i.e. 3.3 in this case
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// create shader object
	Shader lightingShader("C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightingShader.vs", "C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightingShader.fs");

	// shader for lights
	Shader lightCubeShader("C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightCubeShader.vs", "C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/lightCubeShader.fs");



	// the model library contains the actual *definition* of what each block model looks like.
	// e.g., a cube is simply a cube, a stair might have many different faces, etc.
	// we use the singleton pattern for a global library, then use getInstance whenever we want it
	auto& model_lib = ModelLibrary::getInstance();

	// the texture atlas contains the actual texture itself, as well as a hash map for accessing 
	// the TextureRegion (uv coords) of a given texture based on the name, i.e. "grass_top" -> (0.5,0)
	// texture atlas testing
	TextureAtlas atlas("textures");

	// fill block registry (manually, for now) with the definitions of possible blocks
	// for textures, we could block_reg.addDefinition({ 3, "dirt", BlockModel::cube, {atlas.getTextureRegion("dirt_top"), atlas.getTextureRegion("dirt_side"), ...} };
	// for a block with different textures for different faces. 
	auto& block_reg = BlockRegistry::getInstance();
	block_reg.addDefinition({ 0, "air", BlockModel::cube, {} });    
	block_reg.addDefinition({ 1, "cobblestone", BlockModel::cube, {atlas.getTextureRegion("cobblestone"), atlas.getTextureRegion("cobblestone"), atlas.getTextureRegion("cobblestone"), atlas.getTextureRegion("cobblestone"), atlas.getTextureRegion("cobblestone"), atlas.getTextureRegion("cobblestone")} });
	block_reg.addDefinition({ 2, "grass", BlockModel::cube, {atlas.getTextureRegion("test2"), atlas.getTextureRegion("test2"), atlas.getTextureRegion("test2"), atlas.getTextureRegion("test2"), atlas.getTextureRegion("test2"), atlas.getTextureRegion("test2") } });
	block_reg.addDefinition({ 3, "red", BlockModel::cube, {atlas.getTextureRegion("test"), atlas.getTextureRegion("test"), atlas.getTextureRegion("test"), atlas.getTextureRegion("test"), atlas.getTextureRegion("test"), atlas.getTextureRegion("test")} });

	// create mesh for light cube, independent of chunks
	Mesh light_mesh(model_lib.getVertices(BlockModel::cube));

	// test chunk creation
	Chunk chunk1;
	// fill with air
	const int CHUNK_SIZE = 32;
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				chunk1.positions[x][y][z] = 0;
			}
		}
	}

	// "place" different blocks
	chunk1.positions[5][5][5] = 1;
	chunk1.positions[6][5][5] = 2;
	chunk1.positions[7][5][5] = 3;
	
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			for (int z = 0; z < 3; ++z) {
				chunk1.positions[10 + x][10 + y][10 + z] = 1;
			}
		}
	}

	// create the mesh (note: we would typically create this mesh then continually update it in the game loop whenever blocks are changed/broken/placed)
	chunk1.updateChunkMesh();


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// calculate new deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// rendering
		glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear the depth buffer before each render iteration specifically

		// update cube light source position
		light_pos = glm::vec3(light_pos.x + 0.25 * sin(glfwGetTime()), light_pos.y, light_pos.z + 0.25 * cos(glfwGetTime()));

		// CUBE ---------------------------------------------------------------------------------------------------------------
		// activate shader when setting uniforms and drawing objects
		lightingShader.use();

		// set the light position and view position 
		lightingShader.setVec3("light.position", light_pos);
		lightingShader.setVec3("view_pos", camera.position);

		// light properties
		glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 diffuse_color = light_color * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f); // low influence

		lightingShader.setVec3("light.ambient", ambient_color);
		lightingShader.setVec3("light.diffuse", diffuse_color);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		lightingShader.setVec3("material.ambient", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("material.diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setFloat("material.shininess", 32.0f);

		// view/projection matrix transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.getViewMatrix();

		// set uniform(s) for these transformations
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// draw chunk
		atlas.atlas->bind();
		chunk1.chunk_mesh.draw();
		
		
		// --------------------------------------------------------------------------------------------------------------------
		
		
		// LIGHT SOURCE -------------------------------------------------------------------------------------------------------
		// use shader
		lightCubeShader.use();

		// set uniform(s) for view and projection transformations (reuse same ones for cube)
		lightCubeShader.setMat4("view", view);
		lightCubeShader.setMat4("projection", projection);

		// world transformation(s)
		model = glm::mat4(1.0f);
		model = glm::translate(model, light_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setMat4("model", model);

		// set color (the color of the cube itself) of light 
		lightCubeShader.setVec3("color", light_color);

		// render the cube
		light_mesh.draw();
		// -------------------------------------------------------------------------------------------------------------------- 
		

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	lightingShader.deleteShader();
	lightCubeShader.deleteShader();

	glfwTerminate(); 
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	const float cameraSpeed = 5.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.processKeyboard(DOWN, deltaTime);
}

void mouseCallback(GLFWwindow* window, double x_pos_in, double y_pos_in) {
	float x_pos = static_cast<float>(x_pos_in);
	float y_pos = static_cast<float>(y_pos_in);

	if (firstMouse) {
		last_x = x_pos;
		last_y = y_pos;
		firstMouse = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos;
	last_x = x_pos;
	last_y = y_pos;

	camera.processMouseMovement(x_offset, y_offset);
}

void scrollCallback(GLFWwindow* window, double x_offset, double y_offset) {
	camera.processMouseScroll(static_cast<float>(y_offset));
}