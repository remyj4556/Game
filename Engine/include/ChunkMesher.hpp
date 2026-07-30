#ifndef CHUNKMESHER_HPP
#define CHUNKMESHER_HPP

#include <vector>
#include "Vertex.hpp"
#include "Chunk.hpp"
#include "BlockDefinition.hpp"
#include "glm/fwd.hpp"
#include "glm/glm.hpp"

#include <utility>
#include <glad/glad.h>
#include <memory>

class ChunkGroup {
	public:
		std::shared_ptr<Chunk> main; // central Chunk to be meshed
		std::shared_ptr<Chunk> left; // -X
		std::shared_ptr<Chunk> right; // +X
		std::shared_ptr<Chunk> bottom; // -Y
		std::shared_ptr<Chunk> top; // +Y
		std::shared_ptr<Chunk> front; // +Z
		std::shared_ptr<Chunk> back; // -Z

		ChunkGroup() : main(nullptr), left(nullptr), right(nullptr), bottom(nullptr), top(nullptr), back(nullptr), front(nullptr) {}

		const block_id_type blockAtLocalPos(int x, int y, int z) const {
			int chunk_size = Chunk::CHUNK_SIZE;
			
			auto target = main;

			if (x < 0) {
				x = chunk_size - 1;
				target = left;
			}
			else if (x >= chunk_size) {
				x = 0;
				target = right;
			}

			else if (y < 0) {
				y = chunk_size - 1;
				target = bottom;
			}
			else if (y >= chunk_size) {
				y = 0;
				target = top;
			}

			else if (z < 0) {
				z = chunk_size - 1;
				target = back;
			}
			else if (z >= chunk_size) {
				z = 0;
				target = front;
			}

			// if target chunk does not exist, return air (i.e., do not cull face)
			if (!target)
				return 0;
			
			return target->getBlockFast(x, y, z);
		}
};


class ChunkMesher {
	private:
		enum class Direction {
			Back,
			Front,
			Left,
			Right,
			Bottom,
			Top,
		};

		glm::ivec3 directions[6] = { {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0} };
		int indices[6] = { 0, 2, 1, 0, 3, 2 };
		glm::ivec3 face_vertices[6][4] = {
			// 0: Back (-Z)
			{ {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0} },

			// 1: Front (+Z)
			{ {1,0,1}, {0,0,1}, {0,1,1}, {1,1,1} },

			// 2: Left (-X)
			{ {0,0,1}, {0,0,0}, {0,1,0}, {0,1,1} },

			// 3: Right (+X)
			{ {1,0,0}, {1,0,1}, {1,1,1}, {1,1,0} },

			// 4: Bottom (-Y)
			{ {0,0,1}, {1,0,1}, {1,0,0}, {0,0,0} },

			// 5: Top (+Y)
			{ {0,1,0}, {1,1,0}, {1,1,1}, {0,1,1} }
		};

		void addBlockFace(int x, int y, int z, Direction direction, block_id_type current_block, std::vector<Vertex>& mesh_vertices, std::vector<GLuint>& mesh_indices) const;

	public:
		// returns pair of Vertices and corresponding Indices
		std::pair<std::vector<Vertex>, std::vector<GLuint>> buildNaiveMesh(ChunkGroup chunks);
		std::pair<std::vector<Vertex>, std::vector<GLuint>> buildGreedyMesh(ChunkGroup chunks);
};

#endif