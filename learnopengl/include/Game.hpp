#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "World.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"


class Game {
	private:
		GLFWwindow* window;
		int screen_width;
		int screen_height;

		// camera
		Camera camera;
		float last_x = screen_width / 2.0f;
		float last_y = screen_height / 2.0f;
		bool firstMouse = true;

		// delta time 
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		World world;
		// ResourceManager resource_manager;
		LightManager light_manager; // TODO: goes into ResourceManager (along with other stuff) eventually.
		Renderer renderer;
		

	public:
		explicit Game(GLFWwindow* window, int width, int height);
		void run();

		// callback dispatchers 
		static void mouseCallbackDispatch(GLFWwindow* window, double xpos, double ypos);
		static void scrollCallbackDispatch(GLFWwindow* window, double xoffset, double yoffset);
		static void framebufferSizeCallbackDispatch(GLFWwindow* window, int width, int height);

		// actual "callback" functions
		void onMouseMove(double x_pos_in, double y_pos_in);
		void onScroll(double y_offset);
		void onResize(int width, int height);

		void processInput();
		
};

#endif