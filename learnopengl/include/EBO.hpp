#ifndef EBO_HPP
#define EBO_HPP

#include <glad/glad.h>


class EBO {
	public:
		EBO(GLuint indices[], GLsizeiptr size, GLenum type);
		void bind();
		void unbind();
		void del();

	private:
		GLuint ID;
};

EBO::EBO(GLuint indices[], GLsizeiptr size, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, type);
}

void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::del() {
	glDeleteBuffers(1, &ID);
}


#endif