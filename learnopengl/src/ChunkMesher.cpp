#include "../include/ChunkMesher.hpp"
#include "../include/Chunk.hpp"
#include "../include/TextureRegion.hpp"
#include "../include/Vertex.hpp"
#include "../include/BlockMeshingContext.hpp"
#include "../include/WorldCoordinates.hpp"
#include "../include/LocalCoordinates.hpp"
#include "../include/GeometryVertex.hpp"
#include "../include/Block.hpp"

#include <vector>


ChunkMesher::MeshData ChunkMesher::build(ChunkGroup chunks, BlockMeshingContext context) {
	// create local vector to store vertices for chunk, this is all the vertices from an entire chunk
	std::vector<Vertex> chunk_vertices;
	
	int chunk_size = Chunk::getChunkSize();
	

	for (int x = 0; x < chunk_size; ++x) {
		for (int y = 0; y < chunk_size; ++y) {
			for (int z = 0; z < chunk_size; ++z) {
				// get the id of the current block
				block_id_type current_block_id = chunks.blockAtLocalPos(x, y, z).id;

				// skip if air
				if (current_block_id == 0) {
					continue;
				}

				// use the id to look up in the registry the model it uses
				auto current_model = context.block_registry.getDefinition(current_block_id).model;

				// get the material definition of the current block
				auto current_material = context.block_registry.getDefinition(current_block_id).material;

				// use the model to get the vertex data from the ModelLibrary
				std::vector<GeometryVertex> model_vertices = context.model_library.getVertices(current_model);

				// iterate over each face of the current block
				for (int face = 0; face < 6; ++face) {
					// if current face is obscured by another block, i.e., not air (could be eventually a transparent material, though), then skip adding face
					bool cull_face = false;

					switch (face) {
					case 0: // back face (-Z)
						cull_face = (chunks.blockAtLocalPos(x, y, z - 1).id != 0);
		
						break;

					case 1: // front face (+Z)
						cull_face = (chunks.blockAtLocalPos(x, y, z + 1).id != 0);

						break;

					case 2: // left face (-X)
						cull_face = (chunks.blockAtLocalPos(x - 1, y, z).id != 0);

						break;

					case 3: // right face (+X)
						cull_face = (chunks.blockAtLocalPos(x + 1, y, z).id != 0);

						break;

					case 4: // bottom face (-Y)
						cull_face = (chunks.blockAtLocalPos(x, y - 1, z).id != 0);

						break;

					case 5: // top face (+Y)
						cull_face = (chunks.blockAtLocalPos(x, y + 1, z).id != 0);

						break;
					}

					if (cull_face) {
						continue;
					}


					// get uv texture region for current face
					TextureRegion region = context.block_registry.getDefinition(current_block_id).textures[face];

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

	ChunkMesher::MeshData mesh_data;
	mesh_data.vertices = chunk_vertices;
	return mesh_data;
}