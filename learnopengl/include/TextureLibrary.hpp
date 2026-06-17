#ifndef TEXTURELIBRARY_HPP
#define TEXTURELIBRARY_HPP

#include <glad/glad.h>
#include <unordered_map>
#include "glm/fwd.hpp"
#include <string>

class TextureLibrary {
private:
	GLuint ID;
	GLsizei texture_width;
	GLsizei texture_height;
	GLsizei texture_count;
	GLint mipmap_level;

	std::unordered_map<std::string, int> texture_name_to_id;

public:
	// constructor takes a path to a directory of textures and builds them into a texture array
	TextureLibrary(const char* path, GLsizei texture_width, GLsizei texture_height, GLint mipmap_level);
	TextureLibrary();
	void bind() const;
	void unbind() const;
	void del();

	int getTextureID(const std::string& texture_name);
};



#endif