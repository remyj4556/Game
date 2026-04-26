#ifndef MODELLIBRARY_HPP
#define MODELLIBRARY_HPP

#include <array>
#include <glm/glm.hpp>
#include <glm/fwd.hpp>

enum class ModelTypes {
	cube,
	stair,
	slab,
	NUM_MODEL_TYPES
};

class ModelLibrary {
	private:
		struct FaceTemplate {
			glm::vec3 positions[6]; // quad
			glm::vec2 local_uvs[6]; // 0-1 UVs
		};

		struct Model {
			FaceTemplate faces[6];
		};

		std::array<Model, static_cast<size_t>(ModelTypes::NUM_MODEL_TYPES)> models;
		Model createCubeModel();

	public:
		ModelLibrary();

		const Model& getModel(ModelTypes model_type) const;
		void populateDefinitions();
};

#endif
