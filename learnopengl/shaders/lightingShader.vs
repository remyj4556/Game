#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in uint aBlockId;
layout (location = 2) in uint aFaceId;
layout (location = 3) in vec2 aLocalUV;

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

	// TODO: BlockRegistry will exist on GPU --> lookup TexCoord using Vertex's aBlockId. Atlas will exist on GPU as well.
	vec2 uv = aLocalUV / 4;
	uv.x += 0.5;
	uv.y += 0.75;
	TexCoord = uv;
	
	// TODO: get these from BlockRegistry probably too
	Shininess = 4.0f;
	SpecStrength = 0.1f;

	FaceId = aFaceId;
}