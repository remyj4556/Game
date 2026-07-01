#version 330 core

struct BlockDefinition {
	ivec4 face_texture_id_0_3;
	ivec2 face_texture_id_4_5;
	vec4 material;
};

layout (location = 0) in uint aData;
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
    } 
	else {
        return block_defs[block_id].face_texture_id_4_5[face_id - 4];
    }
}

void main() {
	// Unpack data
	int data_x = (int(aData)) & 0x3F;
	int data_y = (int(aData) >> 6) & 0x3F;
	int data_z = (int(aData) >> 12) & 0x3F;
	int data_face_id = (int(aData) >> 18) & 0x7;
	int data_block_id = (int(aData) >> 21) & 0x3FFF;

	vec3 pos = vec3(float(data_x), float(data_y), float(data_z));

	// Vertex Position
	gl_Position = projection * view * model * vec4(pos, 1.0);
	FragPos = vec3(model * vec4(pos, 1.0));
	
	SpecStrength = block_defs[data_block_id].material.x;
	Shininess = block_defs[data_block_id].material.y;
	
	FaceId = data_face_id;
	FaceTextureId = get_face_texture_id(data_block_id, data_face_id);
}