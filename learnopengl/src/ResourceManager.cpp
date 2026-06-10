#include "../include/ResourceManager.hpp"
#include "../include/BlockMeshingContext.hpp"
#include "../include/TextureAtlas.hpp"
#include "../include/GPUBlockDefinition.hpp"
#include <vector>
#include <string>

ResourceManager::ResourceManager() : block_atlas("textures") {
	block_registry.populateDefinitions("json/blocks.json", block_atlas);
	model_library.populateDefinitions();
}

BlockMeshingContext ResourceManager::getBlockMeshingContext() const {
	BlockMeshingContext context = { block_registry, model_library, block_atlas };
	return context;
}

const TextureAtlas* ResourceManager::getTextureAtlas() const {
	return &block_atlas;
}

std::vector<GPUBlockDefinition> ResourceManager::fetchGPUBlockDefinitions() const {
	return block_registry.buildGPUBlockDefinitions();
}