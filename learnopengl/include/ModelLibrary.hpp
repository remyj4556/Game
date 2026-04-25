#ifndef MODELLIBRARY_HPP
#define MODELLIBRARY_HPP

#include <array>

#include "GeometryVertex.hpp"
#include "BlockDefinition.hpp"

class ModelLibrary {
	private:
		struct FaceTemplate {
			glm::vec3 positions[6]; // quad
			glm::vec2 local_uvs[6]; // 0-1 UVs
		};

		struct Model {
			FaceTemplate faces[6];
		};

		std::array<Model, BlockModelCount> models;
		Model createCubeModel();

	public:
		ModelLibrary();

		const Model& getModel(BlockModel model_type) const;
		void populateDefinitions();
};

#endif
