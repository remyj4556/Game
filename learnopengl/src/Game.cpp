#include "../include/Game.hpp"

Game::Game(GLFWwindow* window) : window(window), camera(glm::vec3(0.0f, 0.0f, 0.0f)), first_mouse(true), 
delta_time(0.0f), last_frame(0.0f), world(resource_manager.getBlockMeshingContext()), renderer(window) 
{
	glfwGetWindowSize(window, &screen_width, &screen_height);
	last_x = screen_width / 2.0f;
	last_y = screen_height / 2.0f;
}

void Game::run() {
	StreamTarget target;
	target.load_radius = 5; // TODO: remove magic number

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// calculate new delta_time
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// update cube light source position
		//light_manager.setPlayerLightPosition(camera.position);

		// input
		processInput();

		// update world (creates new chunks, handles block placing, etc.)
		target.pos = camera.position;
		world.update(target);

		// rendering
		renderer.beginFrame(camera, light_manager, resource_manager.getTextureAtlas());

		for (Chunk* chunk : world.getVisibleChunks(target)) {
			renderer.renderChunk(*chunk);
		}


		// LIGHT SOURCE
		//if (light_manager.getPlayerLight().enabled) {
		//	renderer.renderDebugLight(light_mesh, light_manager.getPlayerLight().position + glm::vec3(1.0f, 0.0f, 0.0f));
		//}

		// check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Game::processInput() {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	const float camera_speed = 5000 * delta_time;
	camera.movement_speed = camera_speed;
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

	camera.processMouseMovement(x_offset, y_offset);
}

void Game::onScroll(double y_offset) {
	camera.processMouseScroll(static_cast<float>(y_offset));
}

void Game::onResize(int width, int height) {
	glViewport(0, 0, width, height);
}
