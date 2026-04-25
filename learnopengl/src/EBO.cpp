#include "../include/EBO.hpp"
#include <vector>

EBO::EBO() {
	glGenBuffers(1, &ID);
}

EBO::~EBO() {
	del();
}

EBO::EBO(GLuint indices[], GLsizeiptr size, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, type);
}

EBO::EBO(std::vector<GLuint> indices, GLenum type) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), type);
}

EBO::EBO(EBO&& other) noexcept : ID(other.ID) {
	other.ID = 0;
}

EBO& EBO::operator=(EBO&& other) noexcept {
	if (this != &other) {
		if (ID != 0) {
			del();
		}

		ID = other.ID;
		other.ID = 0;
	}
	return *this;
}

void EBO::bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::del() {
	if (ID != 0) {
		glDeleteBuffers(1, &ID);
		ID = 0;
	}
}

const GLuint EBO::getID() const {
	return ID;
}