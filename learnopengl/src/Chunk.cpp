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

	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				// get the id of the current block
				int current_block_id = positions[x][y][z];

				// skip if air
				if (current_block_id == 0) {
					continue;
				}

				// use the id to look up in the registry the model it uses
				BlockModel current_model = BlockRegistry::getInstance().getDefinition(current_block_id).model;

				// get the material definition of the current block
				MaterialProperties current_material = BlockRegistry::getInstance().getDefinition(current_block_id).material;

				// use the model to get the vertex data from the ModelLibrary
				std::vector<GeometryVertex> model_vertices = ModelLibrary::getInstance().getVertices(current_model);

				// iterate over each face of the current block
				for (int face = 0; face < 6; ++face) {
					// if current face is obscured by another block, i.e., not air (could be eventually a transparent material, though), then skip adding face
					bool cull_face = false;

					switch (face) {
					case 0: // back face (-Z)
						cull_face = (z - 1 >= 0 && positions[x][y][z - 1] != 0);
						break;

					case 1: // front face (+Z)
						cull_face = (z + 1 < CHUNK_SIZE && positions[x][y][z + 1] != 0);
						break;

					case 2: // left face (-X)
						cull_face = (x - 1 >= 0 && positions[x - 1][y][z] != 0);
						break;

					case 3: // right face (+X)
						cull_face = (x + 1 < CHUNK_SIZE && positions[x + 1][y][z] != 0);
						break;

					case 4: // bottom face (-Y)
						cull_face = (y - 1 >= 0 && positions[x][y - 1][z] != 0);
						break;

					case 5: // top face (+Y)
						cull_face = (y + 1 < CHUNK_SIZE && positions[x][y + 1][z] != 0);
						break;
					}

					if (cull_face) {
						continue;
					}


					// get uv texture region for current face
					TextureRegion region = BlockRegistry::getInstance().getDefinition(current_block_id).textures[face];

					// get the next 6 (num vertices per face) vertices from the model_vertices
					for (int vertex = 0; vertex < 6; ++vertex) {
						GeometryVertex geo_vert = model_vertices[6 * face + vertex];
						Vertex vert;

						// apply position offset
						vert.position = geo_vert.position + glm::vec3(x, y, z);

						// assign normal
						vert.normal = geo_vert.normal;

						// apply texture coordinates
						vert.tex_coords.x = region.uv_min.x + geo_vert.tex_coords.x * (region.uv_max.x - region.uv_min.x);
						vert.tex_coords.y = region.uv_min.y + geo_vert.tex_coords.y * (region.uv_max.y - region.uv_min.y);

						// add material properties
						vert.shininess = current_material.shininess;
						vert.specular_strength = (current_material.specular.r + current_material.specular.g + current_material.specular.b) / 3;
						
						chunk_vertices.push_back(vert);
					}
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