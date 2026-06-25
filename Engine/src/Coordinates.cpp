#include "../include/Coordinates.hpp"

#include <iostream>

namespace CoordinateSystem {
	std::ostream& operator<<(std::ostream& os, const CoordinateSystem::ChunkCoordinates& coordinates) {
		os << "x: " << coordinates.x << ", y: " << coordinates.y << ", z: " << coordinates.z;
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const CoordinateSystem::LocalCoordinates& coordinates) {
		os << "x: " << coordinates.x << ", y: " << coordinates.y << ", z: " << coordinates.z;
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const CoordinateSystem::WorldCoordinates& coordinates) {
		os << "x: " << coordinates.x << ", y: " << coordinates.y << ", z: " << coordinates.z;
		return os;
	}
}