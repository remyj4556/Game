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

	public:
		VAO vao;
		VBO vbo;

		Mesh();
		Mesh(const std::vector<Vertex>& vertices_in, const std::vector<Texture>& textures_in);
		~Mesh();

		// move constructor and move assignment, this allows us to properly handle the vao/vbo especially (via their move assignments)
		// because they handle dynamic (gpu) resources through their ID's
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;
		
		// delete the standard copy constructor and copy assignment, to prevent making copies/references to the same mesh, and therefore
		// likely double deleting
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		void buildMesh();
		void draw(Shader& shader);
		void printInfo();
};

#endif