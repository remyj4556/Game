#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>

#include "../include/Renderer.hpp"
#include "../include/Mesh.hpp"
#include "../include/Camera.hpp"
#include "../include/ModelLibrary.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/Chunk.hpp"


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

	// setup renderer
	Renderer renderer(SCR_WIDTH, SCR_HEIGHT);
	

	// temp --------------------------------------------------------------------
	// create mesh for light cube, independent of chunks
	Mesh light_mesh(ModelLibrary::getInstance().getVertices(BlockModel::cube));
	// light source position
	glm::vec3 light_pos(0.0f, 5.0f, 0.0f);

	// test chunk creation
	Chunk chunk1;
	chunk1.chunk_position = glm::vec3(0, 0, 0);

	// fill with air
	const int CHUNK_SIZE = 32;
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				chunk1.positions[x][y][z] = 0;
			}
		}
	}

	// place different blocks
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

	for (int x = 0; x < 10; ++x) {
		for (int z = 0; z < 10; ++z) {
			chunk1.positions[x][0][z] = 1;
		}
	}

	// create the mesh (note: we would typically create this mesh then continually update it in the game loop whenever blocks are changed/broken/placed)
	chunk1.updateChunkMesh();

	// test second chunk creation
	Chunk chunk2;
	chunk2.chunk_position = glm::vec3(0, 0, -32);

	// fill with air
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				chunk2.positions[x][y][z] = 0;
			}
		}
	}

	for (int x = 0; x < 32; ++x) {
		for (int z = 0; z < 32; ++z) {
			chunk2.positions[x][0][z] = 1;
		}
	}

	// create the mesh (note: we would typically create this mesh then continually update it in the game loop whenever blocks are changed/broken/placed)
	chunk2.updateChunkMesh();
	// -------------------------------------------------------------------------


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// calculate new deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// update cube light source position
		light_pos = glm::vec3(light_pos.x + 0.5 * sin(glfwGetTime()), light_pos.y + 0.5 * sin(glfwGetTime()), light_pos.z + 0.5 * cos(glfwGetTime()));

		// input
		processInput(window);

		// rendering
		renderer.beginFrame(camera, light_pos);

		// CUBE
		renderer.renderChunk(chunk1);
		renderer.renderChunk(chunk2);
		
		// LIGHT SOURCE
		renderer.renderLight(light_mesh, light_pos);
		

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate(); 
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	const float cameraSpeed = 10.0f * deltaTime;
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