#ifndef CHUNKCOORDINATES_HPP
#define CHUNKCOORDINATES_HPP

#include <cstdint>
#include <iostream>

class ChunkCoordinates {
	public:
		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;

		bool operator==(const ChunkCoordinates &other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		friend std::ostream& operator<<(std::ostream& os, const ChunkCoordinates& coordinates);
};



static struct CoordinatesHash {
	size_t operator()(const ChunkCoordinates& c) const noexcept {
		// 64-bit mix; works fine even if int32 inputs
		uint64_t x = (uint32_t)c.x;
		uint64_t y = (uint32_t)c.y;
		uint64_t z = (uint32_t)c.z;

		uint64_t h = x * 0x9E3779B185EBCA87ULL;
		h ^= y * 0xC2B2AE3D27D4EB4FULL + (h << 6) + (h >> 2);
		h ^= z * 0x165667B19E3779F9ULL + (h << 6) + (h >> 2);
		return (size_t)h;
	}
};


#endif