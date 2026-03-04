#ifndef MODELLIBRARY_HPP
#define MODELLIBRARY_HPP

#include <vector>
#include <unordered_map>

#include "GeometryVertex.hpp"
#include "BlockDefinition.hpp"

class ModelLibrary {
	private:
		std::unordered_map<BlockModel, std::vector<GeometryVertex>> model_defs;

		inline static const std::vector<GeometryVertex> cube_vertices = {
			// Back face
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
			{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

			// Front face
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

			// Left face
			{{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.0f, 1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

			// Right face
			{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

			// Bottom face
			{{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
			{{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

			// Top face
			{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
		};

	public:
		ModelLibrary();

		const std::vector<GeometryVertex>& getVertices(BlockModel model_type) const;
		void populateDefinitions();
};

#endif
