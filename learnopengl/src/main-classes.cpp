#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../include/shader.hpp"
#include "../include/VBO.hpp"
#include "../include/VAO.hpp"
#include "../include/EBO.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


int main() {
	glfwInit();
	// below two lines ensure that the correct OpenGL version is present, i.e. 3.3 in this case
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);


	// create shader object
	Shader newShader("C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/shader.vs", "C:/Users/remyj/source/repos/learnopengl/learnopengl/shaders/shader.fs");


	// Triangle
	// -----------------------------------------------
	// set up vertex data and buffers
	GLfloat vertices[] = {
		// positions        // colors
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // bottom right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    // bottom left
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f     // top
	};

	// 1. create VAO
	VAO vao;
	vao.bind();

	// 2. create VBO
	VBO vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);

	// 3. tell openGL how to read vertex data : note the difference being the offset and the layout index
	// position attribute
	vao.linkAttrib(vbo, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);

	// color attribute
	vao.linkAttrib(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// 4. unbind for safety
	vbo.unbind();
	vao.unbind();

	// -----------------------------------------------

	// toggle wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// render loop
	float move = 0;
	while (!glfwWindowShouldClose(window)) {
		// input
		processInput(window);

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// use shader
		newShader.use();

		// update the uniform to move vertices
		newShader.setFloat("xOffset", sin(move));
		move += 0.01;

		// draw triangle
		vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	newShader.deleteShader();
	vao.del();
	vbo.del();


	glfwTerminate(); 
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}