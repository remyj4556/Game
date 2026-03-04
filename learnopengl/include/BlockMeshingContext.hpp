#ifndef BLOCKMESHINGCONTEXT_HPP
#define BLOCKMESHINGCONTEXT_HPP

#include "BlockRegistry.hpp"
#include "ModelLibrary.hpp"
#include "TextureAtlas.hpp"

struct BlockMeshingContext {
	const BlockRegistry& block_registry;
	const ModelLibrary& model_library;
	const TextureAtlas& block_atlas;
};

#endif