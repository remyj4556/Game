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


#endif
