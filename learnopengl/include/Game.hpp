#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "World.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "LightManager.hpp"


class Game {
	private:
		GLFWwindow* window;
		int screen_width;
		int screen_height;

		// camera
		Camera camera;
		float last_x;
		float last_y;
		bool first_mouse;

		// delta time 
		float delta_time;
		float last_frame;
		float game_time;

		ResourceManager resource_manager;
		World world;
		LightManager light_manager; 
		Renderer renderer;

		// current stream target acts as the "main player", can be switched out however to allow for multiple cameras/players/perspectives
		StreamTarget current_stream_target;

		// sets the current StreamTarget render distance to provided value (in chunks)
		// maintains invariant that OpenGL render distance is proportional
		void setTargetRenderDistance(int value);
		

	public:
		explicit Game(GLFWwindow* window);
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