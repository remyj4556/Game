#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 view_pos; 

uniform sampler2D texture1;


void main() {
	// ambient lighting
	vec3 ambient = light.ambient * material.ambient;

	// diffuse lighting
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light.position - FragPos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular lighting
	vec3 view_dir = normalize(view_pos - FragPos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	// result
	vec3 result = ambient + diffuse + specular;
	FragColor = mix(texture(texture1, TexCoord), vec4(result, 1.0), 0.5);
}