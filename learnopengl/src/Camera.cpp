#include "../include/Camera.hpp"

Camera::Camera(glm::vec3 position_in, glm::vec3 up_in, GLfloat yaw_in, GLfloat pitch_in)
	: position(position_in),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	world_up(up_in),
	yaw(yaw_in),
	pitch(pitch_in),
	movement_speed(SPEED),
	mouse_sensitivity(SENSITIVITY),
	fov(FOV)
{
	updateCameraVectors();
}

Camera::Camera(GLfloat pos_x, GLfloat pos_y, GLfloat pos_z, GLfloat up_x, GLfloat up_y, GLfloat up_z, GLfloat yaw_in, GLfloat pitch_in)
	: position(glm::vec3(pos_x, pos_y, pos_z)),
	front(glm::vec3(0.0f, 0.0f, -1.0f)),
	world_up(glm::vec3(up_x, up_y, up_z)),
	yaw(yaw_in),
	pitch(pitch_in),
	movement_speed(SPEED),
	mouse_sensitivity(SENSITIVITY),
	fov(FOV)
{
	updateCameraVectors();
}


glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
	float velocity = movement_speed * deltaTime;
	if (direction == FORWARD)
		position += glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * velocity;
	if (direction == BACKWARD)
		position -= glm::normalize(glm::vec3(front.x, 0.0f, front.z)) * velocity;
	if (direction == LEFT)
		position -= glm::normalize(glm::vec3(right.x, 0.0f, right.z)) * velocity;
	if (direction == RIGHT)
		position += glm::normalize(glm::vec3(right.x, 0.0f, right.z)) * velocity;
	if (direction == UP)
		position += glm::normalize(glm::vec3(0.0f, up.y, 0.0f)) * velocity;
	if (direction == DOWN)
		position -= glm::normalize(glm::vec3(0.0f, up.y, 0.0f)) * velocity;
}

void Camera::processMouseMovement(GLfloat x_offset, GLfloat y_offset, GLboolean constrain_pitch) {
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;

	yaw += x_offset;
	pitch += y_offset;

	// constrain
	if (constrain_pitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	// update front, right, up vectors using updated angles
	updateCameraVectors();
}

void Camera::processMouseScroll(GLfloat y_offset) {
	fov -= (float)y_offset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 100.0f)
		fov = 100.0f;
}

void Camera::updateCameraVectors() {
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	new_front.y = sin(glm::radians(pitch));
	new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(new_front);
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}
