#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Camera.hpp"
#include "Chunk.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include "LightManager.hpp"
#include "Mesh.hpp"
#include "GLFW/glfw3.h"
#include "TextureLibrary.hpp"
#include "GPUBlockDefinition.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include <vector>

class Renderer {
	private:
		int screen_width;
		int screen_height;

		float render_distance;

		Shader block_shader;
		Shader light_shader;

		const glm::vec4 DEFAULT_COLOR = glm::vec4(0.53f, 0.81f, 0.92f, 1.0f);
		glm::vec4 clear_color; // sky color

		glm::mat4 projection;
		glm::mat4 view;
		

	public:
		Renderer(GLFWwindow *window);
		~Renderer();

		void beginFrame(Camera& camera, LightManager &light_manager, const TextureLibrary& texture_library);
		void renderChunk(Chunk& chunk);
		void endFrame();

		void setClearColor(glm::vec4 color);
		glm::vec4 getClearColor() const;
		glm::vec4 getDefaultClearColor() const;

		// visualized dynamic debug light
		void renderDebugLight(Mesh& light_mesh, const glm::vec3& light_pos);

		// upload UBO containing block definitions to GPU
		void uploadGPUBlockDefinitions(std::vector<GPUBlockDefinition> gpu_definitions) const;

		void setRenderDistance(float value);
		float getRenderDistance() const;
};



#endif