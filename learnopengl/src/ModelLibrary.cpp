#include "../include/ModelLibrary.hpp"

const std::vector<Vertex>& ModelLibrary::getVertices(BlockModel model_type) {
	return model_defs.at(model_type);
}

void ModelLibrary::populateDefinitions() {
	model_defs[BlockModel::cube] = cube_vertices;
}