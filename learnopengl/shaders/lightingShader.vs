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
	BlockDefinition block_defs[255]; // TODO: change magic number here
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

	vec2 uv;
	vec2 atlas_region_min = vec2(block_defs[aBlockId].face_uv[aFaceId].x, block_defs[aBlockId].face_uv[aFaceId].y);
	vec2 atlas_region_max = vec2(block_defs[aBlockId].face_uv[aFaceId].z, block_defs[aBlockId].face_uv[aFaceId].w);
	uv.x = atlas_region_min.x + aLocalUV.x * (atlas_region_max.x - atlas_region_min.x);
	uv.y = atlas_region_min.y + aLocalUV.y * (atlas_region_max.y - atlas_region_min.y);
	TexCoord = uv;
	
	SpecStrength = block_defs[aBlockId].material.x;
	Shininess = block_defs[aBlockId].material.y;
	
	FaceId = aFaceId;
}