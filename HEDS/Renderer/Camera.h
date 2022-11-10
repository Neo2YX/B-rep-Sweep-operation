#pragma once
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

enum CAMERA_MOVE {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	HIGH,
	LOW
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSETIVITY = 0.1f;
const float FOV = 45.0f;

class Camera
{
public:
	glm::vec3 cameraPosition;
	glm::vec3 cameraFront;
	glm::vec3 worldUp;
	glm::vec3 Right;
	glm::vec3 cameraUp;
	float Pitch, Yaw;
	float fov, cameraSpeed;
	float MouseSensitivity;
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), fov(FOV), cameraSpeed(SPEED), MouseSensitivity(SENSETIVITY)
	{
		cameraPosition = position;
		worldUp = up;
		Pitch = pitch;
		Yaw = yaw;
		updateCamera();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		: cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), fov(FOV), cameraSpeed(SPEED), MouseSensitivity(SENSETIVITY)
	{
		cameraPosition = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		Pitch = pitch;
		Yaw = yaw;
		updateCamera();
	}
	void ProcessKeyboard(CAMERA_MOVE move, float deltaTime);
	void ProcessMouseMove(float xoffset, float yoffset, GLboolean constrainPitch);
	void ProcessMouseScroll(float yoffset);

	glm::mat4 GetViewMatrix();
	float GetFOV() { return fov; }
	glm::vec3 GetPosition() const { return cameraPosition; }
	glm::vec3 GetFront() const { return cameraFront; }
private:
	void updateCamera();
	glm::mat4 LookAt(glm::vec3 position, glm::vec3 target, glm::vec3 up);
};

class FPSCamera : public Camera
{
public:
	FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		: Camera(position, up, yaw, pitch)
	{
		
	}
	void ProcessKeyboard(CAMERA_MOVE move, float deltaTime);
};