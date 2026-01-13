#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include <glm/glm.hpp>

struct DirectionalLight {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	bool enabled = true;
};

struct PointLight {
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	float radius;
	bool enabled = false;
};

class LightManager {
	private:
		DirectionalLight sun;
		DirectionalLight moon;
		PointLight player_light; // torch, lantern, etc.

		float time_of_day = 0.5f;

	public:
		LightManager();

		// update lighting based on time of day
		void update(float delta_time);

		// getter functions
		const DirectionalLight& getSun() const;
		const DirectionalLight& getMoon() const;
		const PointLight& getPlayerLight() const;

		// control player light
		void setPlayerLightPosition(const glm::vec3& pos);
		void enablePlayerLight(bool enabled);

		// time of day controls
		void setTimeOfDay(float time);
		float getTimeOfDay() const;
};



#endif