#ifndef VBO_HPP
#define VBO_HPP

#include <vector>
#include <glad/glad.h>


class VBO {
	public:
		VBO(GLfloat vertices[], GLsizeiptr size, GLenum type);
		VBO(std::vector<GLfloat>& vertices, GLsizeiptr size, GLenum type);
		template <typename T>
		VBO(std::vector<T>& vertices, GLenum type);
		VBO();
		void bind();
		void unbind();
		void del();

	private:
		GLuint ID;
};

VBO::VBO() {
	glGenBuffers(1, &ID);
}

VBO::VBO(GLfloat vertices[], GLsizeiptr size, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, type);
}

VBO::VBO(std::vector<GLfloat> & vertices, GLsizeiptr size, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, &vertices, type);
}

template <typename T>
VBO::VBO(std::vector<T>& vertices, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), type);
}

void VBO::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::del() {
	glDeleteBuffers(1, &ID);
}



#endif