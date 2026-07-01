#include "../include/Mesh.hpp"
#include "../include/Vertex.hpp"
#include <vector>
#include <utility>
#include <iostream>
#include <cstddef>
#include "../include/VBO.hpp"
#include "../include/VAO.hpp"
#include "../include/EBO.hpp"

Mesh::Mesh() {}

Mesh::Mesh(const std::vector<Vertex>& vertices_in, const std::vector<GLuint>& indices_in) : vertices(vertices_in), indices(indices_in) {
	buildMesh();
}

Mesh::~Mesh() {
	vao.del();
	vbo.del();
}

Mesh::Mesh(Mesh&& other) noexcept {
	vao = std::move(other.vao);
	vbo = std::move(other.vbo);
	ebo = std::move(other.ebo);
	vertices = std::move(other.vertices);
	indices = std::move(other.indices);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this != &other) {
		vao = std::move(other.vao);
		vbo = std::move(other.vbo);
		ebo = std::move(other.ebo);
		vertices = std::move(other.vertices);
		indices = std::move(other.indices);
	}
	return *this;
}

void Mesh::buildMesh() {
	if (vertices.empty()) {
		return;
	}

	vao.bind();
	vbo = VBO(vertices, GL_STATIC_DRAW);
	ebo = EBO(indices, GL_STATIC_DRAW);

	vao.linkAttribInt(vbo, 0, 1, GL_UNSIGNED_INT, sizeof(Vertex), (void*)0);

	vbo.unbind();
	vao.unbind();
}

void Mesh::draw() const {
	// draw the mesh
	vao.bind();

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	vao.unbind();
}

void Mesh::printInfo() {
	std::cout << "Mesh vertices: \n";
	for (auto& vert : vertices) {
		std::cout << vert.getX() << " " << vert.getY() << " " << vert.getZ() << "\n";
	}
}


// a Mesh is what we use to actually render. It simply contains Vertex(es), and we create a VBO/VAO for them
// and send them to the GPU within this class. It does not matter what we give it, all it knows is to create
// a VBO for the data stored in the member vector(s). 
