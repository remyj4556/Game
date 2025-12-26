#include "../include/Mesh.hpp"

Mesh::Mesh() {}

Mesh::Mesh(const std::vector<Vertex>& vertices_in) : vertices(vertices_in) {
	buildMesh();
}

Mesh::~Mesh() {
	vao.del();
	vbo.del();
}

Mesh::Mesh(Mesh&& other) noexcept {
	vao = std::move(other.vao);
	vbo = std::move(other.vbo);
	vertices = std::move(other.vertices);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
	if (this != &other) {
		vao = std::move(other.vao);
		vbo = std::move(other.vbo);
		vertices = std::move(other.vertices);
	}
	return *this;
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
	int layout_loc_shininess = 3;
	int layout_loc_specular = 4;

	// position attribute
	vao.linkAttrib(vbo, layout_loc_pos, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	// normal attribute
	vao.linkAttrib(vbo, layout_loc_norm, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// texture attribute
	vao.linkAttrib(vbo, layout_loc_tex, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
	// shininess attribute
	vao.linkAttrib(vbo, layout_loc_shininess, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, shininess));
	// specular attribute
	vao.linkAttrib(vbo, layout_loc_specular, 1, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, specular_strength));

	// 4. unbind for safety
	vbo.unbind();
	vao.unbind();
}

void Mesh::draw() {
	// draw the mesh
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

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
// a VBO for the data stored in the member vector(s). In our case, we want to create a Mesh out of Chunk data,
// not each individual block, nor the entire world.
