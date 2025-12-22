#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Mesh.hpp"
#include "BlockRegistry.hpp"
#include "BlockDefinition.hpp"
#include "ModelLibrary.hpp"
#include "Vertex.hpp"
#include "TextureRegion.hpp"

#include <iostream>
#include <glm/glm.hpp>


class Chunk {
	private:
		static constexpr int CHUNK_SIZE = 32;

	public:
		int positions[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
		glm::vec3 chunk_position;
		// change the single chunk_mesh into opaque_mesh, transparent_mesh, cutout_mesh, etc.
		// these are rendered separately in the render loop, and within the meshes we can still
		// have different materials (sampled in the shader), for, say, reflectivity of metal vs. wood
		Mesh chunk_mesh;

		void updateChunkMesh();
		void printChunkVertices();
};

#endif
