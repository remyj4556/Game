#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include "glm/fwd.hpp"
#include "glm/matrix.hpp"

#include <string>
#include <filesystem>

class Shader {
	public:
		// constructor reads and builds the shader
		Shader(const std::filesystem::path& vertex_path, const std::filesystem::path& fragment_path);
		Shader();
		~Shader();

		GLuint getId();
		void use();
		void deleteShader();

		// utility uniform functions
		void setBool(const std::string &name, bool value) const;
		void setInt(const std::string &name, int value) const;
		void setFloat(const std::string &name, float value) const;
		void setVec2(const std::string& name, const glm::vec2& value) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec3(const std::string& name, const glm::vec3& value) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec4(const std::string& name, const glm::vec4& value) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setMat2(const std::string& name, const glm::mat2& mat) const;
		void setMat3(const std::string& name, const glm::mat3& mat) const;
		void setMat4(const std::string& name, const glm::mat4& mat) const;

		// TODO: can maybe pass data here from renderer function too, as we do above for single uniforms
		void linkUBO(const std::string& name) const;

	private:
		// shader program id
		GLuint ID;

		void checkCompileErrors(GLuint id, const std::string& type) const;
};


#endif