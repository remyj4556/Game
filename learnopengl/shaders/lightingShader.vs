#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aShininess;
layout (location = 4) in float aSpecStrength;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out float Shininess;
out float SpecStrength;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main() {
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoord = aTexCoord;
	Shininess = aShininess;
	SpecStrength = aSpecStrength;
}