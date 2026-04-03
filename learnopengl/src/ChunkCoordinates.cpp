#include "../include/ChunkCoordinates.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const Coordinates& coordinates) {
	os << "x: " << coordinates.x << ", y: " << coordinates.y << ", z: " << coordinates.z;
	return os;
}