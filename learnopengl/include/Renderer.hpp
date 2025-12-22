#ifndef RENDERER_HPP
#define RENDERER_HPP

// renderer owns all shaders, uniforms, etc. This includes (currently) light cube shader, lighting shader, future ones may be water shader, reflective glass, etc.

// renderer acts as a simple API, such as:
//		renderer.beginFrame();
//		renderer.renderChunk();
//		renderer.endFrame();
//		...

// renderer stores the global rendering state as well, like:
//	- projection matrix
//	- view matrix
//  - texture atlases
//  - shaders
//  - ...

#include "Camera.hpp"
#include "Chunk.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "BlockRegistry.hpp"
#include "ModelLibrary.hpp"
#include "Mesh.hpp"

class Renderer {
	private:
		const unsigned int screen_width;
		const unsigned int screen_height;

		// im gonna assume the lack of texture here is from the way we default construct, then assign (without proper assignment operators) the three members below
		Shader block_shader;
		Shader light_shader;
		TextureAtlas block_atlas;

		glm::mat4 projection;
		glm::mat4 view;
		

	public:
		Renderer(const unsigned int screen_width, const unsigned int screen_height);
		~Renderer();

		void beginFrame(Camera& camera, glm::vec3& light_pos);
		void renderChunk(Chunk& chunk);
		void renderLight(Mesh& light_mesh, glm::vec3& light_pos);
		void endFrame();

};



#endif