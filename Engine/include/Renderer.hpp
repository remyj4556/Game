#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <vector>
#include <queue>
#include <unordered_map>

#include "Camera.hpp"
#include "Shader.hpp"
#include "LightManager.hpp"
#include "Mesh.hpp"
#include "GLFW/glfw3.h"
#include "TextureLibrary.hpp"
#include "GPUBlockDefinition.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include "Paths.hpp"
#include "ChunkCoordinates.hpp"
#include "ChunkRenderRequest.hpp"

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

		// references to queues owned by Game
		std::queue<ChunkRenderRequest>& chunk_render_queue;
		std::queue<ChunkCoordinates>& chunk_unload_queue;

		// storage for meshes of all loaded chunks
		std::unordered_map<ChunkCoordinates, Mesh, CoordinatesHash> chunk_meshes;

	public:
		Renderer(GLFWwindow *window, std::queue<ChunkRenderRequest>& chunk_render_queue, std::queue<ChunkCoordinates>& chunk_unload_queue, const Paths& paths);
		~Renderer();

		void beginFrame(Camera& camera, LightManager &light_manager, const TextureLibrary& texture_library);
		void processQueuedChunkMeshes();
		void drawChunks();
		void unloadChunkMesh(ChunkCoordinates chunk_coord);

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