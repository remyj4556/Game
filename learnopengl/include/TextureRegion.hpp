#ifndef TEXTUREREGION_HPP
#define TEXTUREREGION_HPP

#include <glm/glm.hpp>

struct TextureRegion {
	glm::vec2 uv_min; // bottom left corner in atlas
	glm::vec2 uv_max; // top right corner in atlas
};

#endif