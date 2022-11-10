#include "Camera.h"
#include "glm/gtc/matrix_transform.hpp"


glm::mat4 Camera::GetViewMatrix()
{
	return LookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}
void Camera::ProcessKeyboard(CAMERA_MOVE move, float deltaTime)
{
	float velocity = cameraSpeed * deltaTime;
	if (move == FORWARD) cameraPosition += velocity * cameraFront;
	if (move == BACKWARD) cameraPosition -= velocity * cameraFront;
	if (move == RIGHT) cameraPosition += velocity * Right;
	if (move == LEFT) cameraPosition -= velocity * Right;
	if (move == HIGH) cameraPosition += velocity * worldUp;
	if (move == LOW) cameraPosition -= velocity * worldUp;
}
void Camera::ProcessMouseMove(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch) {
		if (Pitch > 89.0f) Pitch = 89.0f;
		if (Pitch < -89.0f) Pitch = -89.0f;
	}
	updateCamera();
}
void Camera::ProcessMouseScroll(float yoffset)
{
	fov -= yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 45.0f) fov = 45.0f;
}

void Camera::updateCamera()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw))*cos(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	cameraFront = glm::normalize(front);
	Right = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(Right, cameraFront));
}

void FPSCamera::ProcessKeyboard(CAMERA_MOVE move, float deltaTime)
{
	float velocity = cameraSpeed * deltaTime;
	if (move == FORWARD) cameraPosition += velocity * glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
	if (move == BACKWARD) cameraPosition -= velocity * glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
	if (move == RIGHT) cameraPosition += velocity * Right;
	if (move == LEFT) cameraPosition -= velocity * Right;
}

glm::mat4 Camera::LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	glm::vec3 cameraAxis = glm::normalize(position - target);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraAxis));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraAxis, cameraRight));
	glm::mat4 translate = glm::mat4(1.0f);
	translate[3][0] = -position.x;
	translate[3][1] = -position.y;
	translate[3][2] = -position.z;
	glm::mat4 rotate = glm::mat4(1.0f);
	rotate[0][0] = cameraRight.x;
	rotate[1][0] = cameraRight.y;
	rotate[2][0] = cameraRight.z;
	rotate[0][1] = cameraUp.x;
	rotate[1][1] = cameraUp.y;
	rotate[2][1] = cameraUp.z;
	rotate[0][2] = cameraAxis.x;
	rotate[1][2] = cameraAxis.y;
	rotate[2][2] = cameraAxis.z;
	return rotate * translate;
}