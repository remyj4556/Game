#ifndef GAME_HPP
#define GAME_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <queue>

#include "World.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "LightManager.hpp"
#include "Paths.hpp"
#include "RenderRequest.hpp"
#include "Coordinates.hpp"
#include "DebugRegistry.hpp"

struct EngineDebugInfo {
	// chunk generation
	float total_gen_time = 0.0f;
	int chunks_generated = 0;

	// meshing
	float total_mesh_time = 0.0f;
	int meshes_generated = 0;

	// rendering
	int chunks_rendered = 0;
	int fps = 0;

	void reset() {
		*this = EngineDebugInfo{};
	}
};

class Game {
	private:
		GLFWwindow* window;
		int screen_width;
		int screen_height;

		Camera camera;
		float last_x;
		float last_y;
		bool first_mouse;

		float delta_time;
		float last_frame;
		float game_time;

		// to coordinate RenderRequest (Chunk Mesh) upload and unload between World and Renderer
		std::queue<RenderRequest> upload_queue;
		std::queue<CoordinateSystem::ChunkCoordinates> unload_queue;
	
		DebugRegistry debug_registry;
		Paths paths;
		ResourceManager resource_manager;
		World world;
		Renderer renderer;
		LightManager light_manager; 
		EngineDebugInfo engine_debug_info;

		// current stream target acts as the "main player", can be switched out however to allow for multiple cameras/players/perspectives
		StreamTarget current_stream_target;

		// sets the current StreamTarget render distance to provided value (in chunks)
		// maintains invariant that OpenGL draw distance (set in Renderer) is proportional
		void setTargetRenderDistance(int value);

	public:
		explicit Game(GLFWwindow* window);
		void run();
		void renderEngineDebugInfo() const;

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