#version 330 core

struct BlockDefinition {
	ivec4 face_texture_id_0_3;
	ivec2 face_texture_id_4_5;
	vec4 material;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aBlockId;
layout (location = 2) in uint aFaceId;
layout (std140) uniform BlockBuffer {
	BlockDefinition block_defs[255];
};

out vec3 FragPos;
out float Shininess;
out float SpecStrength;
flat out int FaceId;
flat out int FaceTextureId;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

int get_face_texture_id(int block_id, int face_id) {
	if (face_id < 4) {
        return block_defs[block_id].face_texture_id_0_3[face_id];
    } else {
        return block_defs[block_id].face_texture_id_4_5[face_id - 4];
    }
}

void main() {
	// Vertex Position
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));

	// signed int safer for GLSL array indexing
	int int_block_id = int(aBlockId);
	int int_face_id = int(aFaceId);
	
	SpecStrength = block_defs[int_block_id].material.x;
	Shininess = block_defs[int_block_id].material.y;
	
	FaceId = int_face_id;
	FaceTextureId = get_face_texture_id(int_block_id, int_face_id);
}