#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

#include "Texture.hpp"
#include "Shader.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Vertex.hpp"


class Mesh {
	private:
		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		VAO vao;
		VBO vbo;

	public:
		Mesh();
		Mesh(std::vector<Vertex>& vertices_in, std::vector<Texture>& textures_in);
		~Mesh();
		void buildMesh();
		void draw(Shader& shader);
};

Mesh::Mesh() {}

Mesh::Mesh(std::vector<Vertex>& vertices_in, std::vector<Texture>& textures_in) : vertices(vertices_in), textures(textures_in) {
	buildMesh();
}

Mesh::~Mesh() {
	vao.del();
	vbo.del();
}

void Mesh::buildMesh() {
	// 1. bind VAO;
	vao.bind();

	// 2. create VBO
	vbo = VBO(vertices, GL_STATIC_DRAW);

	// 3. tell openGL how to read vertex data
	int layout_loc_pos = 0;
	int layout_loc_norm = 1;
	int layout_loc_tex = 2;

	// position attribute
	vao.linkAttrib(vbo, layout_loc_pos, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	// normal attribute
	vao.linkAttrib(vbo, layout_loc_norm, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// texture attribute
	vao.linkAttrib(vbo, layout_loc_tex, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

	// 4. unbind for safety
	vbo.unbind();
	vao.unbind();
}

void Mesh::draw(Shader& shader) {
	for (int i = 0; i < textures.size(); i++) {
		// set texture
		glActiveTexture(GL_TEXTURE0 + i);
		textures[i].bind();
	}
	// draw the mesh
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	vao.unbind();
}


// a Mesh is what we use to actually render. It simply contains Vertex(es), and we create a VBO/VAO for them
// and send them to the GPU within this class. It does not matter what we give it, all it knows is to create
// a VBO for the data stored in the member vector(s). In our case, we want to create a Mesh out of Chunk data,
// not each individual block, nor the entire world.

#endif