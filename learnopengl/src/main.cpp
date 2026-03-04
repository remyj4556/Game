#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../include/Game.hpp"


const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

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

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	// setup light manager
	//LightManager light_manager;
	//light_manager.enablePlayerLight(false);

	
	// temp --------------------------------------------------------------------
	// create mesh for light cube, independent of chunks
	/*
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
	*/

	Game game(window);

	// attach game to the window so callbacks can find it
	glfwSetWindowUserPointer(window, &game);
	glfwSetFramebufferSizeCallback(window, Game::framebufferSizeCallbackDispatch);
	glfwSetCursorPosCallback(window, Game::mouseCallbackDispatch);
	glfwSetScrollCallback(window, Game::scrollCallbackDispatch);

	game.run();

	glfwTerminate();
	return 0;
}





