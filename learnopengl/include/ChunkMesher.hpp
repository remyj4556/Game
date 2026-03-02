#ifndef CHUNKMESHER_HPP
#define CHUNKMESHER_HPP

#include <vector>
#include "Vertex.hpp"
#include "Chunk.hpp"

struct MeshData {
	std::vector<Vertex> vertices;
	//std::vector<uint32_t> indices;
};

// Chunk Mesher basically acts as just a utility function, taking in a Chunk and creating its mesh.
// TODO: this will need to eventually have neighboring chunks passed in, to cull chunk borders
// TODO: this should also not create a Mesh itself. it should return CPU data (vertices, etc), which the renderer
// then uses to build each mesh from probably a 
class ChunkMesher {
	public:
		MeshData build(Chunk &chunk);
};

#endif