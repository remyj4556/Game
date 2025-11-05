#include "../include/ModelLibrary.hpp"

const std::vector<Vertex>& ModelLibrary::getVertices(BlockModel model_type) {
	switch (model_type) {
	case BlockModel::cube:
		return cube_vertices;
	}
}