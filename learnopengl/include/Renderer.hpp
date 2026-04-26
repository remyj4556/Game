#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Camera.hpp"
#include "Chunk.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include "LightManager.hpp"
#include "Mesh.hpp"
#include "GLFW/glfw3.h"
#include "TextureAtlas.hpp"
#include "GPUBlockDefinition.hpp"
#include <glm/glm.hpp>
#include <glm/fwd.hpp>
#include <vector>

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

		// visualized dynamic debug light
		void renderDebugLight(Mesh& light_mesh, const glm::vec3& light_pos);

		// upload UBO containing block definitions to GPU
		void uploadGPUBlockDefinitions(std::vector<GPUBlockDefinition> gpu_definitions) const;
};



#endif