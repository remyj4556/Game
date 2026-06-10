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

in vec3 FragPos;
in vec2 TexCoord;
in float Shininess;
in float SpecStrength;
flat in uint FaceId;

out vec4 FragColor;

uniform DirectionalLight sun;
uniform PointLight player_light;
uniform float time_of_day;
uniform vec3 view_pos; 
uniform sampler2D texture1; // Diffuse Texture (albedo)
// can add another texture here for Specular Texture

const vec3 normals[6] = vec3[](
    vec3(0,0,-1),
    vec3(0,0,1),
    vec3(-1,0,0),
    vec3(1,0,0),
    vec3(0,-1,0),
    vec3(0,1,0)
);

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
    float dist2 = dot(to_light, to_light);
    float radius2 = light.radius * light.radius;

    vec3 light_dir = normalize(to_light);

    // attenuation (light falloff)
    float attenuation = clamp(1.0 - dist2 / radius2, 0.0, 1.0);
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
	vec3 norm = normalize(normals[FaceId]);
	vec3 view_dir = normalize(view_pos - FragPos);

	vec3 result = vec3(0.0);
    
	if (sun.enabled) {
        result += calculateDirectionalLight(sun, norm, view_dir, tex_color) * time_of_day;
    }

    if (player_light.enabled) {
        result += calculatePointLight(player_light, norm, FragPos, view_dir, tex_color);
    }

    result += 0.2 * tex_color;
    
    FragColor = vec4(result, 1.0);
}