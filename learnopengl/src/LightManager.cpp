#include "../include/LightManager.hpp"

LightManager::LightManager() {
	// initialize sun
	sun.direction = glm::normalize(glm::vec3(0.3f, -0.8f, 0.2f));
	sun.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	sun.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	sun.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	sun.enabled = true;

	// initialize moon
	moon.direction = glm::normalize(glm::vec3(-0.3f, -0.5f, -0.2f));
	moon.ambient = glm::vec3(0.01f, 0.015f, 0.02f);
	moon.diffuse = glm::vec3(0.1f, 0.15f, 0.2f);
	moon.specular = glm::vec3(0.3f, 0.35f, 0.4f);
	moon.enabled = false;

	// initialize player light (disabled on start)
	player_light.position = glm::vec3(0.0f);
	player_light.color = glm::vec3(1.0f, 0.7f, 0.4f); 
	player_light.intensity = 1.5f;
	player_light.radius = 8.0f;
	player_light.enabled = false;
}

void LightManager::update(float delta_time) {

}

const DirectionalLight& LightManager::getSun() const {
	return sun;
}
const DirectionalLight& LightManager::getMoon() const {
	return moon;
}

const PointLight& LightManager::getPlayerLight() const {
	return player_light;
}

void LightManager::setPlayerLightPosition(const glm::vec3& pos) {
	player_light.position = pos;
}

void LightManager::enablePlayerLight(bool enabled) {
	player_light.enabled = enabled;
}

void LightManager::setTimeOfDay(float time) {
	time_of_day = time;
}

float LightManager::getTimeOfDay() const {
	return time_of_day;
}