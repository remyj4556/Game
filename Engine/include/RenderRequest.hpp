#ifndef RENDER_REQUEST
#define RENDER_REQUEST

#include "Coordinates.hpp"
#include "Mesh.hpp"

struct RenderRequest {
	CoordinateSystem::ChunkCoordinates chunk_coord;
	Mesh chunk_mesh;
};

#endif