#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Block.hpp"
#include "Mesh.hpp"

const int CHUNK_SIZE = 10;

class Chunk {
	private:
		Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

		// updateMesh iterates over every block in the chunk and adds visible ones' vertices to the vector to be rendered
		// gets called by draw()
		void updateMesh();
		
		glm::vec3 position = glm::vec3(1.0f);

	public:
		// mesh stores the VAO and VBO of a chunk. it can be accessed multiple times, and only needs to be
		// updated when a block changes. it is updated via updateMesh()
		Mesh mesh;

		// eventually we will generate a chunk using a different class, like WorldGenerator or something
		// i *think* a way to do that is to construct a chunk using some subset of a noise map we get
		// from the WorldGenerator. right now, simply define a chunk manually
		Chunk();

		// getters
		const Block * getBlocks();

		void draw(Shader &shader);
};

Chunk::Chunk() {
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				Block block(BlockType::Cobblestone, glm::vec3((float)i, (float)j, (float)k));
				blocks[i][j][k] = block;
			}
		}
	}
	updateMesh();
}

const Block * Chunk::getBlocks() {
	return &blocks[0][0][0];
}

void Chunk::updateMesh() {
	// clear old vertices from mesh
	mesh.vertices.clear();

	std::vector<glm::vec3> cube_vertices = {
		// Front face
		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),

		// Back face
		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),

		// Left face
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),

		// Right face
		glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f),

		// Top face
		glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f),

		// Bottom face
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f)
	};

	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			for (int k = 0; k < CHUNK_SIZE; ++k) {
				// check visibility of current block
				// for right now just add all vertices to vector
				glm::vec3 block_position = blocks[i][j][k].getPos();

				for (const auto& vertex_position : cube_vertices) {
					Vertex vertex;
					vertex.position = vertex_position + block_position;
					vertex.tex_coords = glm::vec2(0.5, 1);
					mesh.vertices.push_back(vertex);
				}
			}
		}
	}
	mesh.generate();
}

void Chunk::draw(Shader &shader) {
	// create model matrix: consists of any translations, scalings, rotations, etc.,
	// that we would apply to transform all object's vertices to *world space*
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	// update uniforms
	shader.setMat4("model", model);

	// render each 
	mesh.vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
}



#endif