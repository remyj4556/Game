#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include "stb_image.h"
#include "Shader.hpp"


class Texture {
	public:
		Texture(const char* path, GLenum textureType, GLenum slot, GLenum imageFormat, GLenum pixelType);
		void bind();
		void unbind();
		void del();
		void textureUnit(Shader& shader, const char* uniform, GLuint unit);

	private:
		GLuint ID;
		GLenum type;
};

Texture::Texture(const char* path, GLenum textureType, GLenum slot, GLenum imageFormat, GLenum pixelType) {
	type = textureType;

	// generate an opengl texture object
	glGenTextures(1, &ID);
	
	// activate texture unit before binding texture, as to bind it to the active unit
	glActiveTexture(slot);
	glBindTexture(type, ID);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// load image
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data) {
		glTexImage2D(type, 0, GL_RGB, width, height, 0, imageFormat, pixelType, data);
		glGenerateMipmap(type);
	}
	else {
		std::cout << "Failed to load texture for ID: " << ID << std::endl;
	}

	stbi_image_free(data);
	unbind();
}

void Texture::textureUnit(Shader& shader, const char* uniform, GLuint unit) {
	shader.use();
	shader.setInt(uniform, unit);
}

void Texture::bind() {
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::del() {
	glDeleteTextures(1, &ID);
}

#endif
