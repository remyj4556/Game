#ifndef EBO_HPP
#define EBO_HPP

#include <glad/glad.h>
#include <vector>

class EBO {
	public:
		EBO(GLuint indices[], GLsizeiptr size, GLenum type);
		EBO(std::vector<GLuint> indices, GLenum type);
		EBO();
		~EBO();

		// move ctor and move assignment operator, to properly handle deep copies via the classes ID's
		EBO(EBO&& other) noexcept;
		EBO& operator=(EBO&& other) noexcept;

		// delete the standard copy ctor and copy assignment operator, to prevent making two EBO classes
		// that point to the same gpu resources/data
		EBO(const EBO&) = delete;
		EBO& operator=(const EBO&) = delete;

		void bind() const;
		void unbind() const;
		void del();

		const GLuint getID() const;

	private:
		GLuint ID = 0;
};



#endif