#include "../include/ModelLibrary.hpp"

ModelLibrary::ModelLibrary() {}

const std::vector<GeometryVertex>& ModelLibrary::getVertices(BlockModel model_type) const {
	return model_defs.at(model_type);
}

void ModelLibrary::populateDefinitions() {
	model_defs[BlockModel::cube] = cube_vertices;
}