#include "../include/Mesh.hpp"
#include "../include/Vertex.hpp"
#include <vector>
#include <utility>
#include <iostream>
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
	// 1. bind VAO;
	vao.bind();

	// 2. create VBO
	vbo = VBO(vertices, GL_STATIC_DRAW);

	// 2.5 create EBO
	ebo = EBO(indices, GL_STATIC_DRAW);

	// 3. tell openGL how to read vertex data
	int layout_loc_pos = 0;
	int layout_loc_id = 1;
	int layout_loc_face = 2;
	int layout_loc_uv = 3;

	// position attribute
	vao.linkAttribFloat(vbo, layout_loc_pos, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	// block id attribute
	vao.linkAttribInt(vbo, layout_loc_id, 1, GL_UNSIGNED_SHORT, sizeof(Vertex), (void*)offsetof(Vertex, id));
	// face id attribute
	vao.linkAttribInt(vbo, layout_loc_face, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void*)offsetof(Vertex, face));
	// local uv attribute
	vao.linkAttribFloat(vbo, layout_loc_uv, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, local_uv));

	// 4. unbind for safety
	vbo.unbind();
	vao.unbind();
}

void Mesh::draw() {
	// draw the mesh
	vao.bind();

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	vao.unbind();
}

void Mesh::printInfo() {
	std::cout << "Mesh vertices: \n";
	for (auto& vert : vertices) {
		std::cout << vert.position.x << " " << vert.position.y << " " << vert.position.z << "\n";
	}
}


// a Mesh is what we use to actually render. It simply contains Vertex(es), and we create a VBO/VAO for them
// and send them to the GPU within this class. It does not matter what we give it, all it knows is to create
// a VBO for the data stored in the member vector(s). 
