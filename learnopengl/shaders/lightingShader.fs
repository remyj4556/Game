#version 330 core

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in float Shininess;
in float SpecStrength;

out vec4 FragColor;

uniform Light light;
uniform vec3 view_pos; 
uniform sampler2D texture1; // Diffuse Texture (albedo)
// can add another texture here for Specular Texture


void main() {
	// get base color from albedo texture
	vec3 tex_color  = texture(texture1, TexCoord).rgb;

	// ambient light derived from texture
	vec3 ambient = light.ambient * tex_color;

	// diffuse lighting
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light.position - FragPos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * tex_color;

	// specular lighting
	vec3 view_dir = normalize(view_pos - FragPos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), Shininess);
	vec3 specular = light.specular * spec * SpecStrength;

	// result
	FragColor = vec4(ambient + diffuse + specular, 1.0);
}