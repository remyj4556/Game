#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/Shader.hpp"
#include "../include/VBO.hpp"
#include "../include/VAO.hpp"
#include "../include/EBO.hpp"
#include "../include/Texture.hpp"
#include "stb_image.h"

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
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	GLuint indices[] = {
		0, 1, 3,			 // first triangle
		1, 2, 3				 // second triangle
	};

	// 1. create VAO
	VAO vao;
	vao.bind();

	// 2. create VBO
	VBO vbo(vertices, sizeof(vertices), GL_STATIC_DRAW);

	EBO ebo(indices, sizeof(indices), GL_STATIC_DRAW);

	// 3. tell openGL how to read vertex data : note the difference being the offset and the layout index
	// position attribute
	vao.linkAttrib(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);

	// color attribute
	vao.linkAttrib(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	
	// texture attribute
	vao.linkAttrib(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// 4. unbind for safety
	vbo.unbind();
	vao.unbind();
	ebo.unbind();

	// -----------------------------------------------

	// toggle wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



	// Textures
	// -----------------------------------------------
	Texture texture1("C:/Users/remyj/source/repos/learnopengl/learnopengl/textures/container.jpg", GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE);
	Texture texture2("C:/Users/remyj/source/repos/learnopengl/learnopengl/textures/awesomeface.png", GL_TEXTURE_2D, 1, GL_RGBA, GL_UNSIGNED_BYTE);

	// tell opengl which texture unit each shader sampler belongs
	texture1.textureUnit(newShader, "texture1", 0);
	texture2.textureUnit(newShader, "texture2", 1);

	// -----------------------------------------------


	// render loop
	while (!glfwWindowShouldClose(window)) {
		// input
		processInput(window);

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// use shader
		newShader.use();

		// transformation matrix
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		// set uniform
		GLuint transformLoc = glGetUniformLocation(newShader.getId(), "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		// draw rectangle
		glActiveTexture(GL_TEXTURE0);
		texture1.bind();
		glActiveTexture(GL_TEXTURE1);
		texture2.bind();

		vao.bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	newShader.deleteShader();
	vao.del();
	vbo.del();
	ebo.del();
	texture1.del();
	texture2.del();


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