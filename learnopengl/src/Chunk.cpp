#include "../include/Chunk.hpp"


// iterate over every position in positions, and get the id of the block.
// look in the block registry, and get its model type. use that to look in the
// model library and get its actual vertex definitions. from the block registry
// also get the texture offset for the block id, and add both of them to the local
// vertex vector and texture vector respectively, and then create a mesh out of them.
void Chunk::updateChunkMesh() {
	// create local vector to store vertices for chunk, this is all the vertices from an entire chunk
	// i.e., potentially: CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 36    (36 being the number for a cube)
	// so possibly 32 * 32 * 32 * 36 = 1179648 vertices. This is why we NEED to minimize the number of them
	// we add to the mesh, as well as the size of the Vertex object itself (could use bytes/chars in the future)
	std::vector<Vertex> chunk_vertices;

	// create a local vector to store all textures found in this chunk
	std::vector<Texture> chunk_textures;
	
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

				//std::cout << "at position: " << i << " " << j << " " << k << "\n is block id: " << current_block_id 
				//		  << "\n with name: " << BlockRegistry::getInstance().getDefinition(current_block_id).display_name << "\n";

				// use the id to look up in the registry the texture and model it uses
				BlockModel current_model = BlockRegistry::getInstance().getDefinition(current_block_id).model;

				if (current_model == BlockModel::cube) {
					//std::cout << " block model: cube\n";
				}

				// use the model to get the vertex data from the ModelLibrary
				std::vector<Vertex> model_vertices = ModelLibrary::getInstance().getVertices(current_model);

				// get the texture for the current block
				chunk_textures.push_back(BlockRegistry::getInstance().getDefinition(current_block_id).texture);

				// idk the best way to move the model_vertices into the chunk_vertices, right now
				// we don't modify them in any way but soon we will, so it might have to be O(n) 
				// here regardless
				for (Vertex& vert : model_vertices) {
					vert.position.x += i;
					vert.position.y += j;
					vert.position.z += k;

					chunk_vertices.push_back(vert);
				}
				
				// IMPORTANT: somewhere we have to offset the vertices for each block by its position, right now they are all
				// at 0,0,0 regardless
			}
		}
	}

	// create the new mesh with the chunks vertices and assign it to the member mesh
	// use temporary texture vector (default init) right now
	std::cout << "Chunk vertices: \n";
	for (auto& vert : chunk_vertices) {
		//std::cout << vert.position.x << " " << vert.position.y << " " << vert.position.z << "\n";
	}
	// almost certain this is the culprit, vao/vbo data is probably being lost after this goes out of scope or smth,
	// need to make copy/move operators for Mesh
	//std::cout << "before reassignment vao: " << chunk_mesh.vao.getID() << "\n";
	//std::cout << "before reassignment vbo: " << chunk_mesh.vbo.getID() << "\n";

	chunk_mesh = std::move(Mesh(chunk_vertices, chunk_textures));

	//std::cout << "after reassignment vao: " << chunk_mesh.vao.getID() << "\n";
	//std::cout << "after reassignment vbo: " << chunk_mesh.vbo.getID() << "\n";
}

void Chunk::printChunkVertices() {
	chunk_mesh.printInfo();
}