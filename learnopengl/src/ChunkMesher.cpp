#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/Vertex.hpp"
#include "../include/BlockMeshingContext.hpp"
#include "../include/Block.hpp"

#include <vector>
#include <utility>
#include <cstdint>


std::pair<std::vector<Vertex>, std::vector<GLuint>> ChunkMesher::buildNaiveMesh(ChunkGroup chunks, BlockMeshingContext context) {
	std::vector<Vertex> mesh_vertices;
	std::vector<GLuint> mesh_indices;

	int chunk_size = Chunk::getChunkSize();

	for (int i = 0; i < (chunk_size * chunk_size * chunk_size); ++i) {
		int x = i % chunk_size;
		int y = (i / chunk_size) % chunk_size;
		int z = ((i / chunk_size) / chunk_size) % chunk_size;

		// get the id of the current block
		Block current_block = chunks.blockAtLocalPos(x, y, z);

		// skip if air
		if (current_block.id == 0) {
			continue;
		}

		// iterate over each face
		Direction face_dir = Direction::Back;
		for (const auto & dir : directions) {
			const Block& face_neighbor = chunks.blockAtLocalPos(x + dir.x, y + dir.y, z + dir.z);

			if (face_neighbor.id == 0) {
				addBlockFace(x, y, z, face_dir, current_block, mesh_vertices, mesh_indices);
			}

			face_dir = static_cast<Direction>(static_cast<int>(face_dir) + 1);
		}
	}

	return { mesh_vertices, mesh_indices };
}

void ChunkMesher::addBlockFace(int x, int y, int z, Direction direction, Block current_block, std::vector<Vertex> &mesh_vertices, std::vector<GLuint> &mesh_indices) const {
	const glm::vec3* face = face_vertices[static_cast<size_t>(direction)];

	
	// add indices
	GLuint offset = static_cast<GLuint>(mesh_vertices.size());
	for (int i = 0; i < 6; ++i) {
		mesh_indices.push_back(indices[i] + offset);
	}

	// LL - LR - TR - TL vertex order
	for (int i = 0; i < 4; ++i) {
		Vertex v;

		// assign position to vertex
		v.position.x = face[i].x + x;
		v.position.y = face[i].y + y;
		v.position.z = face[i].z + z;

		// assign block id to vertex
		v.id = current_block.id;

		// assign face id to vertex
		v.face = static_cast<uint8_t>(direction);

		// assign local uv to vertex
		v.local_uv = face_local_uvs[i];

		// add vertex to mesh
		mesh_vertices.push_back(v);
	}
}

