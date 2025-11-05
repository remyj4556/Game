#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>
#include <vector>

#include "Vertex.hpp"

class VBO {
	public:
		VBO(GLfloat vertices[], GLsizeiptr size, GLenum type);
		VBO(std::vector<Vertex> &vertices, GLenum type);
		VBO();
		void bind();
		void unbind();
		void del();
		const GLuint getID();

	private:
		GLuint ID;
};



#endif