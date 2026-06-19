#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include "BlockRegistry.hpp"
#include "ModelLibrary.hpp"
#include "GPUBlockDefinition.hpp"
#include "TextureLibrary.hpp"
#include "Paths.hpp"
#include <vector>

class ResourceManager {
	private:
		TextureLibrary texture_library;
		BlockRegistry block_registry;
		ModelLibrary model_library;

	public:
		ResourceManager(const Paths &paths);
		const std::vector<GPUBlockDefinition> fetchGPUBlockDefinitions() const;
		const TextureLibrary& getTextureLibrary() const;
};


#endif