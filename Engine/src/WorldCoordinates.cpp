#include "../include/WorldCoordinates.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const WorldCoordinates& coordinates) {
	os << "x: " << coordinates.x << ", y: " << coordinates.y << ", z: " << coordinates.z;
	return os;
}