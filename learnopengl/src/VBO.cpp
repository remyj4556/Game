#include "../include/VBO.hpp"

VBO::VBO() {}

VBO::VBO(GLfloat vertices[], GLsizeiptr size, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, type);
}

VBO::VBO(std::vector<Vertex> &vertices, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), type);
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

const GLuint VBO::getID() {
	return ID;
}

