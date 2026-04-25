#include "../include/ModelLibrary.hpp"
#include "../include/BlockDefinition.hpp"

ModelLibrary::ModelLibrary() {}

const ModelLibrary::Model& ModelLibrary::getModel(BlockModel model_type) const {
	return models[static_cast<size_t>(model_type)];
}

void ModelLibrary::populateDefinitions() {
    models[static_cast<size_t>(BlockModel::cube)] = createCubeModel();
}

ModelLibrary::Model ModelLibrary::createCubeModel() {
    Model cube;

    //cube.faces[0].positions
    //cube.faces[1] = makeFrontFace();
    //cube.faces[2] = makeLeftFace();
    //cube.faces[3] = makeRightFace();
    //cube.faces[4] = makeBottomFace();
    //cube.faces[5] = makeTopFace();

    return cube;
}