#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Coordinates.hpp"
#include "BlockDefinition.hpp"
#include <array>
#include <string>
#include <atomic>

enum class ChunkState {
	Unloaded,
	QueuedToLoad, // any chunks that exist but do not contain data yet, will either be generated or loaded from disk
	Loaded,		  
	QueuedToMesh, // functions as a "dirty" flag
	Meshed,
	Uploaded,
	QueuedToUnload // any state can be queued to unload, thus not used in advance state function
};

class Chunk {
	public:
		Chunk();
		static constexpr int CHUNK_SIZE = 32;
		
		std::atomic<ChunkState> state;

		// block access
		const block_id_type getBlock(CoordinateSystem::LocalCoordinates coordinates) const;
		void setBlock(CoordinateSystem::LocalCoordinates coordinates, block_id_type block);
		inline int index(int x, int y, int z) const {
			return x + (z * CHUNK_SIZE) + (y * (CHUNK_SIZE * CHUNK_SIZE));
		}
		inline block_id_type getBlockFast(int x, int y, int z) const {
			return blocks[x + (z * CHUNK_SIZE) + (y * (CHUNK_SIZE * CHUNK_SIZE))];
		}

		void advanceChunkState();
		std::string chunkStateToString() const;
		static std::string chunkStateToString(ChunkState chunk_state);

	private:
		std::array<block_id_type, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks;
};

#endif
