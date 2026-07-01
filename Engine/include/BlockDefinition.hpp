#ifndef BLOCKDEFINITION_HPP
#define BLOCKDEFINITION_HPP

#include <string>
#include <unordered_map>
#include <cstdint>

using block_id_type = uint16_t;

struct MaterialProperties {
	float specular = 0.0f;
	float shininess = 0.0f;
};

struct BlockDefinition {
	block_id_type id;
	bool is_transparent = false;
	int face_texture_id[6];
	MaterialProperties material;
};

#endif