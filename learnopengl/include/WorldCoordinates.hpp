#ifndef WORLDCOORDINATES_HPP
#define WORLDCOORDINATES_HPP

#include <cstdint>
#include <iostream>

class WorldCoordinates {
	public:
		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;

		bool operator==(const WorldCoordinates& other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		friend std::ostream& operator<<(std::ostream& os, const WorldCoordinates& coordinates);
};

#endif