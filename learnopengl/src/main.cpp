#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <unordered_map>

#include "../include/Renderer.hpp"
#include "../include/Mesh.hpp"
#include "../include/Camera.hpp"
#include "../include/ModelLibrary.hpp"
#include "../include/BlockDefinition.hpp"
#include "../include/Chunk.hpp"
#include "../include/LightManager.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, LightManager& light_manager);
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

	// setup light manager
	LightManager light_manager;
	light_manager.enablePlayerLight(false);


	// temp --------------------------------------------------------------------
	// create mesh for light cube, independent of chunks
	std::vector<GeometryVertex> light_geo_verts = ModelLibrary::getInstance().getVertices(BlockModel::cube);
	std::vector<Vertex> light_verts;
	for (GeometryVertex& geo_vert : light_geo_verts) {
		Vertex vert;
		vert.normal = geo_vert.normal;
		vert.position = geo_vert.position;
		vert.tex_coords = geo_vert.tex_coords;
		vert.shininess = 0.0f;
		vert.specular_strength = 0.0f;
		light_verts.push_back(vert);
	}
	Mesh light_mesh(light_verts);


	// TODO: testing chunk dynamic building (NOTE: using int for map right now. we have single dimension right now)
	int num_chunks = 10;
	const int CHUNK_SIZE = 32;
	std::unordered_map<int, Chunk*> chunk_map;

	for (int i = 0; i < num_chunks; ++i) {
		// test chunk creation
		//Chunk chunk1;
		//chunk1.chunk_position = glm::vec3(0, 0, 0);

		// create new chunk at location 0 + i (x coord, test for now). TODO: i think we should use "chunk coords" not in terms of block coordinates. i.e, shouldn't
		// need to multiply by 32 here
		int pos = 0 + 32 * i;
		Chunk* chunk1 = new Chunk;
		chunk1->chunk_position = glm::vec3(pos, 0, 0);

		// fill with air 
		/*
		for (int x = 0; x < CHUNK_SIZE; ++x) {
			for (int y = 0; y < CHUNK_SIZE; ++y) {
				for (int z = 0; z < CHUNK_SIZE; ++z) {
					chunk1->positions[x][y][z] = 0;
				}
			}
		}*/

		// place different blocks
		chunk1->positions[5][5][5] = 1;
		chunk1->positions[6][5][5] = 2;
		chunk1->positions[7][5][5] = 3;

		for (int x = 0; x < 3; ++x) {
			for (int y = 0; y < 3; ++y) {
				for (int z = 0; z < 3; ++z) {
					chunk1->positions[10 + x][10 + y][10 + z] = 4;
				}
			}
		}

		for (int x = 0; x < 10; ++x) {
			for (int z = 0; z < 10; ++z) {
				chunk1->positions[x][0][z] = 3;
			}
		}

		chunk_map[pos] = chunk1;
	}


	// create the meshes (note: we would typically create this mesh then continually update it in the game loop whenever blocks are changed/broken/placed)
	for (auto& chunk : chunk_map) {
		std::cout << chunk.second->chunk_position.x << std::endl;
		chunk.second->updateChunkMesh();
	}


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// calculate new deltaTime
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// update cube light source position
		light_manager.setPlayerLightPosition(camera.position);

		// input
		processInput(window, light_manager);

		// rendering
		renderer.beginFrame(camera, light_manager);

		// CUBE
		// testing chunk map
		for (auto& chunk : chunk_map) {
			renderer.renderChunk(*(chunk.second));
		}


		// LIGHT SOURCE
		if (light_manager.getPlayerLight().enabled) {
			renderer.renderDebugLight(light_mesh, light_manager.getPlayerLight().position + glm::vec3(1.0f, 0.0f, 0.0f));
		}

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

// TODO: temporarily pass light_manager for torch enable/disable keybind, eventually
// change to class member
void processInput(GLFWwindow* window, LightManager& light_manager) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	const float camera_speed = 500 * deltaTime;
	camera.movement_speed = camera_speed;
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
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (light_manager.getPlayerLight().enabled)
			light_manager.enablePlayerLight(false);
		else
			light_manager.enablePlayerLight(true);
	}
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