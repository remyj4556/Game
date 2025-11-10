#ifndef VAO_HPP
#define VAO_HPP

#include <glad/glad.h>
#include "VBO.hpp"


class VAO {
	public:
		VAO();
		~VAO();
		
		// move ctor and move assignment operator, to properly handle deep copies via the classes ID's
		VAO(VAO&& other) noexcept;
		VAO& operator=(VAO&& other) noexcept;

		// delete the standard copy ctor and copy assignment operator, to prevent making two VAO classes
		// that point to the gpu resources/data
		VAO(const VAO&) = delete;
		VAO& operator=(const VAO&) = delete;

		void linkAttrib(VBO &vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		void bind();
		void unbind();
		void del();
		const GLuint getID();

	private:
		GLuint ID = 0;
};



#endif