#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

#include "Shader.hpp"
#include "VBO.hpp"
#include "VAO.hpp"
#include "EBO.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec2 tex_coords;
};

class Mesh {
	public:
		VAO vao;
		VBO vbo;
		std::vector<Vertex> vertices;

		Mesh();
		~Mesh();
		void generate();
};

Mesh::Mesh() {}

Mesh::~Mesh() {
	vao.del();
	vbo.del();
}

void Mesh::generate() {
	// 1. bind vao
	vao.bind();

	// 2. create new vbo and assign
	vbo = VBO(vertices, GL_STATIC_DRAW);

	// 3. tell openGL how to read vertex data : note the difference being the offset and the layout index
	// position attribute
	vao.linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));

	// texture attribute
	vao.linkAttrib(vbo, 1, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

	// 4. unbind for safety
	vbo.unbind();
	vao.unbind();
}

#endif