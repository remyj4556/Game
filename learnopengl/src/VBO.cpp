#include "../include/VBO.hpp"

VBO::VBO() {
	glGenBuffers(1, &ID);
}

VBO::~VBO() {
	del();
}

VBO::VBO(VBO&& other) noexcept : ID(other.ID) {
	other.ID = 0;
}

VBO& VBO::operator=(VBO&& other) noexcept {
	if (this != &other) {
		if (ID != 0) {
			del();
		}

		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}

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
	if (ID != 0) {
		glDeleteBuffers(1, &ID);
		ID = 0;
	}
}

const GLuint VBO::getID() {
	return ID;
}

