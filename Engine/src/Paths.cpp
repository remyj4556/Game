#include "../include/Paths.hpp"
#include "../include/whereami.h"
#include <filesystem>

Paths::Paths() {
	std::filesystem::path exe_dir = getExecutableDirectory();
	assets = exe_dir / "assets";
	shaders = assets / "shaders";
	textures = assets / "textures";
	json = exe_dir / "json";
}

std::filesystem::path Paths::getExecutableDirectory() {
	// get executable location
	int length = wai_getExecutablePath(NULL, 0, NULL);
	char* path = new char[length + 1];
	int dirname_length;
	wai_getExecutablePath(path, length, &dirname_length);
	path[length] = '\0';

	// get parent folder of this location
	std::filesystem::path exe_dir = std::filesystem::path(std::string(path)).parent_path();
	delete[] path;

	return exe_dir;
}