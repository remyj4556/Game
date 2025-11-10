#ifndef VBO_HPP
#define VBO_HPP

#include <glad/glad.h>
#include <vector>

#include "Vertex.hpp"

class VBO {
	public:
		VBO(GLfloat vertices[], GLsizeiptr size, GLenum type);
		VBO(std::vector<Vertex> &vertices, GLenum type);
		VBO();
		~VBO();

		// move ctor and move assignment operator, to properly handle deep copies via the classes ID's
		VBO(VBO&& other) noexcept;
		VBO& operator=(VBO&& other) noexcept;

		// delete the standard copy ctor and copy assignment operator, to prevent making two VBO classes
		// that point to the gpu resources/data
		VBO(const VBO&) = delete;
		VBO& operator=(const VBO&) = delete;

		void bind();
		void unbind();
		void del();
		const GLuint getID();

	private:
		GLuint ID = 0;
};



#endif