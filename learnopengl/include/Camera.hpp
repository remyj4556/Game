#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// defines options for camera movement
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 5.0f;
const GLfloat SENSITIVITY = 0.1f;
const GLfloat FOV = 80.0f;


class Camera {
	public:	
		// camera attributes
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 world_up;

		// euler angles
		GLfloat yaw;
		GLfloat pitch;

		// camera options
		GLfloat movement_speed;
		GLfloat mouse_sensitivity;
		GLfloat fov;

		// constructor with vectors
		Camera(glm::vec3 position_in = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up_in = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw_in = YAW, GLfloat pitch_in = PITCH);

		// constructor with scalar values
		Camera(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat up_x, GLfloat up_y, GLfloat up_z, GLfloat yaw_in, GLfloat pitch_in);

		// returns the view matrix calculated using euler angles and lookAt matrix
		glm::mat4 getViewMatrix();

		// processes input received from mouse
		void processMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch = true);

		// processes input received from keyboard
		void processKeyboard(CameraMovement direction, float deltaTime);

		// processes input received from mouse scroll-wheel
		void processMouseScroll(GLfloat y_offset);

	private:
		void updateCameraVectors();
};



#endif