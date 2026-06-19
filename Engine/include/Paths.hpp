#ifndef PATHS_HPP
#define PATHS_HPP

#include <filesystem>

class Paths {
	private:
		std::filesystem::path getExecutableDirectory();

	public:
		std::filesystem::path assets;
		std::filesystem::path shaders;
		std::filesystem::path textures;
		std::filesystem::path json;

		Paths();
};

#endif