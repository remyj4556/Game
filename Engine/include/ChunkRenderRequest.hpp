#ifndef CHUNK_RENDER_REQUEST
#define CHUNK_RENDER_REQUEST

#include "ChunkCoordinates.hpp"
#include "Mesh.hpp"

struct ChunkRenderRequest {
	ChunkCoordinates chunk_coord;
	Mesh chunk_mesh;
};

#endif