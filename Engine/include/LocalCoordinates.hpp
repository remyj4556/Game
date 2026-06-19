#ifndef LOCALCOORDINATES_HPP
#define LOCALCOORDINATES_HPP

#include <cstdint>
#include <iostream>

class LocalCoordinates {
	public:
		uint8_t x;
		uint8_t y;
		uint8_t z;

		bool operator==(const LocalCoordinates &other) const {
			return x == other.x && y == other.y && z == other.z;
		}

		friend std::ostream& operator<<(std::ostream& os, const LocalCoordinates& coordinates);
};


#endif