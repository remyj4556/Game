#include "../include/ResourceManager.hpp"
#include "../include/TextureLibrary.hpp"
#include "../include/GPUBlockDefinition.hpp"
#include "../include/Paths.hpp"
#include <vector>

ResourceManager::ResourceManager(const Paths &paths) 
	: texture_library(paths.textures, 16, 16, 4)
{
	block_registry.populateDefinitions(paths.json / "blocks.json", texture_library);
	model_library.populateDefinitions();
}

const std::vector<GPUBlockDefinition> ResourceManager::fetchGPUBlockDefinitions() const {
	return block_registry.buildGPUBlockDefinitions();
}

const TextureLibrary& ResourceManager::getTextureLibrary() const {
	return texture_library;
}