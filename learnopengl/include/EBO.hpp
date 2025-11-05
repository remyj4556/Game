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



#endif