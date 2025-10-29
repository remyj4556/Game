#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "../include/Mesh.hpp"
#include "../include/Shader.hpp"
#include "../include/VBO.hpp"
#include "../include/VAO.hpp"
#include "../include/EBO.hpp"
#include "../include/Texture.hpp"
#include "../include/Camera.hpp"
#include "../include/Vertex.hpp"
#include "../include/BlockRegistry.hpp"
#include "../include/ModelLibrary.hpp"
#include "stb_image.h"

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
	// we instantiate the library once, and then use it to build meshes.
	ModelLibrary model_lib;

	// the block registry holds the information for what a block is, but not its vertices or rendering data.
	// here we assign cobblestone to be ID of 0 and a cube shape. we would ideally do this in a loop, reading
	// from a json containing all blocks in the game, adding them to the registry upon startup. then, whenever 
	// we need to access a block's info, we use the registry. block registry will also contain coordinates to
	// a texture atlas, right now simply 
	BlockRegistry block_reg;
	block_reg.addDefinition({ 0, "cobblestone", BlockModel::cube });


	// load textures
	Texture texture1("C:/Users/remyj/source/repos/Game2/learnopengl/textures/cobblestone.png", GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE);
	texture1.textureUnit(lightingShader, "texture1", 0);
	Texture texture2("C:/Users/remyj/source/repos/Game2/learnopengl/textures/container.jpg", GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE);
	texture2.textureUnit(lightingShader, "texture2", 1);

	std::vector<Texture> textures = { texture1, texture2 };

	// create mesh for cube and light. note: we *should* create a mesh for each entire chunk of cubes, not just the vertices of a single one
	// this would be done not here in main but rather in Chunk or something, updating only when a block is broken/placed. An interesting thing
	// is that we pass in a vector of textures, but how do we know which texture goes to which block in the chunk? well, we will eventually have
	// a class for a Block, and thus every block is not a mesh but a Block. then, when creating a mesh for a Chunk, we iterate over every block,
	// and can access its ID. we then use the ID to look it up in the registry, and find its necessary texture, etc.
	Mesh cube_mesh(model_lib.getCubeVertices(), textures);
	Mesh cube_mesh2(model_lib.getCubeVertices(), textures);
	Mesh light_mesh(model_lib.getCubeVertices(), textures);

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
		cube_mesh.draw(lightingShader);

		model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setMat4("model", model);
		cube_mesh2.draw(lightingShader);
		
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
		light_mesh.draw(lightCubeShader);
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