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

	public:
		Mesh();
		Mesh(const std::vector<Vertex>& vertices_in, const std::vector<Texture>& textures_in);
		~Mesh();
		void buildMesh();
		void draw(Shader& shader);
		void printInfo();
		/*Mesh& operator=(const Mesh& rhs) {
			if (this == &rhs) {
				return *this;
			}

			vao.del();
			vbo.del();

			vao = rhs.vao;
			vbo = rhs.vbo;

			return *this;
		}*/
};

#endif