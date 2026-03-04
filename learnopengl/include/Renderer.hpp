#ifndef RENDERER_HPP
#define RENDERER_HPP

// renderer owns all shaders, uniforms, etc. This includes (currently) light cube shader, lighting shader, future ones may be water shader, reflective glass, etc.

// renderer acts as a simple API, such as:
//		renderer.beginFrame();
//		renderer.renderChunk();
//		renderer.endFrame();
//		...

#include "Camera.hpp"
#include "Chunk.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "LightManager.hpp"
#include "Mesh.hpp"
#include "GLFW/glfw3.h"
#include "TextureAtlas.hpp"

class Renderer {
	private:
		int screen_width;
		int screen_height;

		float render_distance = 1000.0f;

		Shader block_shader;
		Shader light_shader;

		glm::mat4 projection;
		glm::mat4 view;
		

	public:
		Renderer(GLFWwindow *window);
		~Renderer();

		void beginFrame(Camera& camera, LightManager &light_manager, const TextureAtlas *block_atlas);
		void renderChunk(Chunk& chunk);
		void endFrame();

		// visualized debug dynamic light
		void renderDebugLight(Mesh& light_mesh, const glm::vec3& light_pos);
		

};



#endif