#version 330 core

struct BlockDefinition {
	vec4 face_uv[6];
	vec4 material;
};

layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aBlockId;
layout (location = 2) in uint aFaceId;
layout (location = 3) in vec2 aLocalUV;
layout (std140) uniform BlockBuffer {
	BlockDefinition block_defs[255];
};

out vec3 FragPos;
out vec2 TexCoord;
out float Shininess;
out float SpecStrength;
flat out uint FaceId;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	// Vertex Position
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));

	// signed int safer for GLSL array indexing
	int int_block_id = int(aBlockId);
	int int_face_id = int(aFaceId);

	// fetch stored atlas region: (min.x, min.y, max.x, max.y)
	vec4 region = block_defs[int_block_id].face_uv[int_face_id];
	vec2 atlas_min = region.xy;
	vec2 atlas_max = region.zw;

	vec2 tiled_uv = aLocalUV;
	TexCoord = atlas_min + tiled_uv * (atlas_max - atlas_min);
	
	SpecStrength = block_defs[int_block_id].material.x;
	Shininess = block_defs[int_block_id].material.y;
	
	FaceId = aFaceId;
}