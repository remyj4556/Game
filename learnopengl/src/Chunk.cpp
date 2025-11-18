#include "../include/Chunk.hpp"


// iterate over every position in positions, and get the id of the block.
// look in the block registry, and get its model type. use that to look in the
// model library and get its actual vertex definitions. from the block registry
// also get the texture offset for the block id, and add both of them to the local
// vertex vector and texture vector respectively, and then create a mesh out of them.
void Chunk::updateChunkMesh() {
	// create local vector to store vertices for chunk, this is all the vertices from an entire chunk
	std::vector<Vertex> chunk_vertices;
	
	// TEST: maximum number of vertices for a single chunk of cubes -- not sure how this affects performance in practice
	//chunk_vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 36);

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				// get the id of the current block
				int current_block_id = positions[i][j][k];

				// skip if air
				if (current_block_id == 0) {
					continue;
				}

				// use the id to look up in the registry the texture and model it uses
				BlockModel current_model = BlockRegistry::getInstance().getDefinition(current_block_id).model;

				// use the model to get the vertex data from the ModelLibrary
				std::vector<Vertex> model_vertices = ModelLibrary::getInstance().getVertices(current_model);

				// get the uv texture coords of each face of the current block, add them to a vector
				std::vector<TextureRegion> face_textures;
				for (int face = 0; face < 6; ++face) {
					TextureRegion region = BlockRegistry::getInstance().getDefinition(current_block_id).textures[face];
					face_textures.push_back(region);
			
				}

				// process vertices
				for (int v = 0; v < model_vertices.size(); ++v) {
					Vertex vert = model_vertices[v];

					// apply position offset
					vert.position.x += i;
					vert.position.y += j;
					vert.position.z += k;

					int face_index = v / 6;    // 6 vertices per face
					TextureRegion& region = face_textures[face_index];

					// apply texture coordinates
					vert.tex_coords.x = region.uv_min.x + vert.tex_coords.x * (region.uv_max.x - region.uv_min.x);
					vert.tex_coords.y = region.uv_min.y + vert.tex_coords.y * (region.uv_max.y - region.uv_min.y);

					chunk_vertices.push_back(vert);
				}
			}
		}
	}

	// create the new mesh with the chunks vertices and assign it to the member mesh
	chunk_mesh = std::move(Mesh(chunk_vertices));
}

void Chunk::printChunkVertices() {
	chunk_mesh.printInfo();
}