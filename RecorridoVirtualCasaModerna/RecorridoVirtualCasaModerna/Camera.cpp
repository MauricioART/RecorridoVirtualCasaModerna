#include "Camera.h"

Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	tipoCamara = 0;
	show = 0;
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
	if (keys[GLFW_KEY_1]) {
		tipoCamara = 1;
	}
	if (keys[GLFW_KEY_2]) {
		tipoCamara = 0;
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
		if (position.y <= 5.25)
		position += glm::vec3(front.x, 0.0f, front.z) * velocity + glm::vec3(0.0f, 0.2f, 0.0f);
	}
	if (keys[GLFW_KEY_F]) {
		if (position.y >= 0.95)
			position -= glm::vec3(front.x, 0.0f, front.z) * velocity + glm::vec3(0.0f, 0.2f, 0.0f);
	}
	if (keys[GLFW_KEY_L]) {
		show = 1;
	}
	if (keys[GLFW_KEY_K]) {
		show = 0;
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

bool Camera::doesItMove() {
	if ( lastPosition == position) 
		return 0;
	else
		return 1;

}
void Camera::setLastPosition(glm::vec3 pos) {
	lastPosition = pos;
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::calculateViewMatrix3p()
{
	glm::vec3 frontBuffer = glm::normalize(front);
	position3p = position - glm::vec3(4*frontBuffer.x, -0.5f, 4*frontBuffer.z);
	return glm::lookAt( position3p,  position3p + front, up);
	//return glm::lookAt( position3p,  glm::vec3(frontBuffer.x, -0.5f, frontBuffer.z), up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}
glm::vec3 Camera::getCameraPosition3p()
{
	glm::vec3 frontBuffer = glm::normalize(front);
	position3p = position - glm::vec3(4* frontBuffer.x, -0.5f, 4 * frontBuffer.z);
	return position3p;
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
