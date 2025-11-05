#ifndef VAO_HPP
#define VAO_HPP

#include <glad/glad.h>
#include "VBO.hpp"


class VAO {
	public:
		VAO();
		void linkAttrib(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		void bind();
		void unbind();
		void del();
		const GLuint getID();

	private:
		GLuint ID;
};



#endif