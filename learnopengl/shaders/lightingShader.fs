#version 330 core

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    bool enabled;
};

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
    bool enabled;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in float Shininess;
in float SpecStrength;
in vec3 BlockLight;

out vec4 FragColor;

uniform DirectionalLight sun;
uniform PointLight player_light;
uniform vec3 view_pos; 
uniform sampler2D texture1; // Diffuse Texture (albedo)
// can add another texture here for Specular Texture

vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 view_dir, vec3 tex_color) {
	vec3 light_dir = normalize(-light.direction);

	// diffuse
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * tex_color;

	// Specular
	vec3 half_dir = normalize(light_dir + view_dir);
	float spec = pow(max(dot(normal, half_dir), 0.0), Shininess);
	vec3 specular = light.specular * spec * SpecStrength;

	return light.ambient * tex_color + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir, vec3 tex_color) {
    vec3 to_light = light.position - frag_pos;
    float distance = length(to_light);
    
    // check for out of range light
    if (distance > light.radius) return vec3(0.0);
    
    vec3 light_dir = normalize(to_light);
    
    // attenuation
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    attenuation *= light.intensity;
    
    // diffuse
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = light.color * diff * tex_color;
    
    // specular
    vec3 half_dir = normalize(light_dir + view_dir);
    float spec = pow(max(dot(normal, half_dir), 0.0), Shininess);
    vec3 specular = light.color * spec * SpecStrength;
    
    return (diffuse + specular) * attenuation;
}


void main() {
	// get base color from albedo texture
	vec3 tex_color  = texture(texture1, TexCoord).rgb;
	vec3 norm = normalize(Normal);
	vec3 view_dir = normalize(view_pos - FragPos);

	vec3 result = vec3(0.0);

	if (sun.enabled) {
        result += calculateDirectionalLight(sun, norm, view_dir, tex_color);
    }

    if (player_light.enabled) {
        result += calculatePointLight(player_light, norm, FragPos, view_dir, tex_color);
    }

    result += BlockLight * tex_color;
    FragColor = vec4(result, 1.0);
}