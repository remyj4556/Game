#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <vector>
#include <unordered_map>
#include <queue> 

#include "concurrentqueue.h"
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
#include "Coordinates.hpp"
#include "RenderRequest.hpp"

struct RendererDebugInfo {

};

class Renderer {
	private:
		int screen_width;
		int screen_height;

		float render_distance;

		Shader block_shader;
		Shader light_shader;

		const glm::vec4 DEFAULT_COLOR = glm::vec4(0.53f, 0.81f, 0.92f, 1.0f);
		glm::vec4 clear_color;

		glm::mat4 projection;
		glm::mat4 view;

		RendererDebugInfo renderer_debug_info;

		// Mesh upload queue reference, owned by Game.
		// state is strictly ChunkState::Meshed
		std::queue<RenderRequest>& upload_queue;

		// Mesh unload queue reference, owned by Game.
		// state is strictly ChunkState::Uploaded
		std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue;

		// storage for meshes of all loaded chunks
		std::unordered_map<CoordinateSystem::ChunkCoordinates, Mesh, CoordinateSystem::ChunkCoordinatesHash> chunk_meshes;

	public:
		Renderer(GLFWwindow *window, std::queue<RenderRequest>& load_queue, std::queue<CoordinateSystem::ChunkCoordinates>& unload_queue, const Paths& paths);
		~Renderer();

		void beginFrame(Camera& camera, LightManager &light_manager, const TextureLibrary& texture_library);
		void processQueuedChunkMeshes();
		void drawChunks();
		void unloadChunkMesh(CoordinateSystem::ChunkCoordinates chunk_coord);
		void setClearColor(glm::vec4 color);
		glm::vec4 getClearColor() const;
		glm::vec4 getDefaultClearColor() const;
		void setRenderDistance(float value);
		float getRenderDistance() const;
		void renderDebugLight(Mesh& light_mesh, const glm::vec3& light_pos);
		void uploadGPUBlockDefinitions(std::vector<GPUBlockDefinition> gpu_definitions) const;
		void renderRendererDebugInfo() const;
};




#endif