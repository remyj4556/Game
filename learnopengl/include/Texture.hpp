#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include "Shader.hpp"
#include "stb_image.h"


class Texture {
	public:
		Texture(const char* path, GLenum textureType, GLenum slot, GLenum imageFormat, GLenum pixelType);
		Texture(GLenum textureType, GLenum slot, GLenum imageFormat, GLenum pixelType);
		void loadTexture(const char* path, GLenum imageFormat, GLenum pixelType);
		void bind();
		void unbind();
		void del();
		void textureUnit(Shader& shader, const char* uniform, GLuint unit);
		const GLuint getID() const;

	private:
		GLuint ID;
		GLenum type;
};


#endif
