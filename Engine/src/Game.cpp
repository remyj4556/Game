#include "../include/Game.hpp"
#include "../include/World.hpp"
#include "../include/Chunk.hpp"
#include "../include/Paths.hpp"
#include "../include/Coordinates.hpp"
#include <vector>
#include <imgui.h>
#include <functional>

Game::Game(GLFWwindow* window)
	: window(window)
	, camera(glm::vec3(0.0f, 0.0f, 0.0f))
	, first_mouse(true)
	, delta_time(0.0f)
	, last_frame(0.0f)
	, game_time(1.0f)
	, resource_manager(paths)
	, world(load_queue, unload_queue)
	, renderer(window, load_queue, unload_queue, paths)
	, current_stream_target(StreamTarget({0.0f, 0.0f, 0.0f}, 12))
{
	glfwGetWindowSize(window, &screen_width, &screen_height);
	last_x = screen_width / 2.0f;
	last_y = screen_height / 2.0f;

	const auto gpu_defs = resource_manager.fetchGPUBlockDefinitions();
	renderer.uploadGPUBlockDefinitions(gpu_defs);

	std::function<void()> f_renderEngineDebugInfo = [this]{ renderEngineDebugInfo(); };
	std::function<void()> f_renderRendererDebugInfo = [this] { renderer.renderRendererDebugInfo(); };
	std::function<void()> f_renderWorldDebugInfo = [this] { world.renderWorldDebugInfo(); };
	debug_registry.registerMember("Game", f_renderEngineDebugInfo);
	debug_registry.registerMember("Renderer", f_renderRendererDebugInfo);
	debug_registry.registerMember("World", f_renderWorldDebugInfo);
}

void Game::run() {
	std::vector<int> fps_arr;
	
	while (!glfwWindowShouldClose(window)) {
		// calculate new delta_time
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// average fps calculation
		fps_arr.push_back(1 / delta_time);
		if (fps_arr.size() == 60) {
			engine_debug_info.fps = 0;
			
			// get average
			float current_fps = 0.0f;
			for (int val : fps_arr) {
				current_fps += val;
			}

			engine_debug_info.fps = current_fps / 60;
			fps_arr.clear();
		}

		// update light source position
		light_manager.setPlayerLightPosition(camera.position);

		// update time of day
		light_manager.setTimeOfDay(game_time);
		renderer.setClearColor(renderer.getDefaultClearColor() * game_time);

		// input
		processInput();

		// update world
		current_stream_target.pos = camera.position;
		world.update(current_stream_target);

		// rendering
		renderer.beginFrame(camera, light_manager, resource_manager.getTextureLibrary());
		renderer.processQueuedChunkMeshes();
		renderer.drawChunks();
		
		// coordinate out of range chunks unloading
		while (!unload_queue.empty()) {
			CoordinateSystem::ChunkCoordinates current_chunk_coord = unload_queue.front();
			unload_queue.pop();

			renderer.unloadChunkMesh(current_chunk_coord);
			world.unloadChunk(current_chunk_coord);
		}
		
		// render debug data
		debug_registry.renderMemberData();

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Game::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	camera.movement_speed = 35;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.processKeyboard(UP, delta_time);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.processKeyboard(DOWN, delta_time);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (light_manager.getPlayerLight().enabled)
			light_manager.enablePlayerLight(false);
		else
			light_manager.enablePlayerLight(true);
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void Game::mouseCallbackDispatch(GLFWwindow* window, double xpos, double ypos) {
	auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (game) {
		game->onMouseMove(xpos, ypos);
	}
}

void Game::scrollCallbackDispatch(GLFWwindow* window, double xoffset, double yoffset) {
	auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (game) {
		game->onScroll(yoffset);
	}
}

void Game::framebufferSizeCallbackDispatch(GLFWwindow* window, int width, int height) {
	auto* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
	if (game) {
		game->onResize(width, height);
	}
}

void Game::onMouseMove(double x_pos_in, double y_pos_in) {
	float x_pos = static_cast<float>(x_pos_in);
	float y_pos = static_cast<float>(y_pos_in);

	if (first_mouse) {
		last_x = x_pos;
		last_y = y_pos;
		first_mouse = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos;
	last_x = x_pos;
	last_y = y_pos;

	// don't move camera if mouse showing
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
		return;
	}
	camera.processMouseMovement(x_offset, y_offset);
}

void Game::onScroll(double y_offset) {
	camera.processMouseScroll(static_cast<float>(y_offset));
}

void Game::onResize(int width, int height) {
	glViewport(0, 0, width, height);
}

void Game::setTargetRenderDistance(int value) {
	current_stream_target.chunk_load_radius = value;
	renderer.setRenderDistance(Chunk::CHUNK_SIZE * (current_stream_target.chunk_load_radius));
}

void Game::renderEngineDebugInfo() const {
	ImGui::Text("Position: %i %i %i", (int)camera.position.x, (int)camera.position.y, (int)camera.position.z);
	ImGui::Text("FPS: %i", engine_debug_info.fps);
}