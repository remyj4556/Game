#include "../include/LocalCoordinates.hpp"

#include <iostream>

std::ostream& operator<<(std::ostream& os, const LocalCoordinates& coordinates) {
	os << "x: " << coordinates.x << ", y: " << coordinates.y << ", z: " << coordinates.z;
	return os;
}