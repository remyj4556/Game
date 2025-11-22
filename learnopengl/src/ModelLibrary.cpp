#include "../include/ModelLibrary.hpp"

const std::vector<Vertex>& ModelLibrary::getVertices(BlockModel model_type) {
	// TODO: do not use a switch here, we want O(1) access. populate a map with the vertex definitions at startup
	switch (model_type) {
	case BlockModel::cube:
		return cube_vertices;
	}
}