#ifndef RENDER_REQUEST
#define RENDER_REQUEST

#include "Coordinates.hpp"
#include "Vertex.hpp"
#include <vector>
#include <glad/glad.h>

struct RenderRequest {
	CoordinateSystem::ChunkCoordinates chunk_coord;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
};

#endif