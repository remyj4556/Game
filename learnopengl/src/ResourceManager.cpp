#include "../include/ResourceManager.hpp"
#include "../include/TextureLibrary.hpp"
#include "../include/GPUBlockDefinition.hpp"
#include <vector>
#include <string>
#include <iostream>

// TODO: don't hardcode texture library sizes
ResourceManager::ResourceManager(const std::string& texture_directory_path) : texture_library(texture_directory_path.c_str(), 16, 16, 4) {
	block_registry.populateDefinitions("json/blocks.json", texture_library);
	model_library.populateDefinitions();
}

const std::vector<GPUBlockDefinition> ResourceManager::fetchGPUBlockDefinitions() const {
	return block_registry.buildGPUBlockDefinitions();
}

const TextureLibrary& ResourceManager::getTextureLibrary() const {
	return texture_library;
}