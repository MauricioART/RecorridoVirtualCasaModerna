#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	bool getTipoCamara() { return tipoCamara; }
	bool doesItMove();
	void setLastPosition(glm::vec3 pos);
	bool getShow() { return show; }
	

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraPosition3p();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();
	glm::mat4 calculateViewMatrix3p();
	~Camera();

private:
	glm::vec3 lastPosition;
	glm::vec3 position;
	glm::vec3 position3p;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	bool tipoCamara;
	bool show;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

