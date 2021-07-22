#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{   
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += glm::vec3(front.x,0.0f,front.z) * velocity;
	}
	if (keys[GLFW_KEY_Q])
	{
		position += glm::vec3(front.x,0.0f,front.z) * velocity;
	}

	if (keys[GLFW_KEY_S])
	{
		position -= glm::vec3(front.x, 0.0f, front.z) * velocity;
	}

	if (keys[GLFW_KEY_A])
	{
		position -= glm::vec3(right.x, 0.0f, right.z) * velocity;
	}

	if (keys[GLFW_KEY_D])
	{
		position += glm::vec3(right.x, 0.0f, right.z) * velocity;
	}
	if (keys[GLFW_KEY_SPACE]) {
		if (position.y <= 5.5)
		position += glm::vec3(front.x, 0.0f, front.z) * velocity + glm::vec3(0.0f, 0.2f, 0.0f);
	}
	if (keys[GLFW_KEY_F]) {
		if (position.y >= 0.2)
			position -= glm::vec3(front.x, 0.0f, front.z) * velocity + glm::vec3(0.0f, 0.2f, 0.0f);
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}


glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}


Camera::~Camera()
{
}
