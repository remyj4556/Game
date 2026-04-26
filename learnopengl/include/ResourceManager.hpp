#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include "BlockRegistry.hpp"
#include "ModelLibrary.hpp"
#include "TextureAtlas.hpp"
#include "BlockMeshingContext.hpp"
#include "GPUBlockDefinition.hpp"
#include <vector>

class ResourceManager {
	private:
		TextureAtlas block_atlas;
		BlockRegistry block_registry;
		ModelLibrary model_library;

	public:
		ResourceManager();
		std::vector<GPUBlockDefinition> fetchGPUBlockDefinitions() const;
		BlockMeshingContext getBlockMeshingContext() const;
		const TextureAtlas* getTextureAtlas() const;
};


#endif