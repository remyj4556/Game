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





