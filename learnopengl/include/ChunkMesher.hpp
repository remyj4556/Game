#ifndef CHUNKMESHER_HPP
#define CHUNKMESHER_HPP

#include <vector>
#include "Vertex.hpp"
#include "Chunk.hpp"
#include "Block.hpp"
#include "BlockMeshingContext.hpp"
#include "LocalCoordinates.hpp"
#include "WorldCoordinates.hpp"

#include <cstdint>

class ChunkGroup {
	public:
		Chunk* main;   // central Chunk to be meshed
		Chunk* left;   // -X
		Chunk* right;  // +X
		Chunk* bottom; // -Y
		Chunk* top;    // +Y
		Chunk* back;   // -Z
		Chunk* front;  // +Z

		ChunkGroup() : main(nullptr), left(nullptr), right(nullptr), bottom(nullptr), top(nullptr), back(nullptr), front(nullptr) {}

		// TODO: may need to work on coordinate system here. Local chunk coordinates are naturally unsigned, but we allow negatives for this function to access outside of the chunk and into neighbors
		const Block blockAtLocalPos(int x, int y, int z) const {
			int chunk_size = Chunk::getChunkSize();
			
			
			Chunk* target = main;

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
				return Block{ 0 };
			
			return target->getBlock({ static_cast<uint8_t>(x), static_cast<uint8_t>(y), static_cast<uint8_t>(z) });
		

			// inside main chunk
			return main->getBlock({ uint8_t(x), uint8_t(y), uint8_t(z) });
		}

};

// Chunk Mesher basically acts as just a utility function, taking in a Chunk and creating its mesh.
// TODO: this will need to eventually have neighboring chunks passed in, to cull chunk borders
class ChunkMesher {
	public:
		struct MeshData {
			std::vector<Vertex> vertices;
			//std::vector<uint32_t> indices;
		};

		MeshData build(ChunkGroup chunks, BlockMeshingContext context);
};

#endif