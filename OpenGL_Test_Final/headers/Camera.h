#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

private:
	float fPitch = 0.0f;
	float fYaw = -90.0f;
	float fFov = 80.0f;

	float fLastX = 400.0f;
	float fLastY = 300.0f;

	//bool bMouseStarted = true;

	glm::mat4 matView;

public:
	float fCameraSpeed = 5.0f;

	Camera() = default;	

	void init(glm::vec3 vPos, glm::vec3 vFront);
	const glm::mat4& getLookAt();

	// We essentially change vCamPos using vCamFront and vCamUp
	void ProcessKeyboard(CameraMovement movement, float fDeltaTime);

	// We essentially change vCamDir using pitch and yaw values
	void ProcessMouse(float fMousePosX, float fMousePosY, int screenWidth, int screenHeight, bool bFirstMouse);

	void SetCameraPos(glm::vec3 vPos);

	void UpdateView(Shader shader, const std::string& viewMat4ID);
};

void Camera::init(glm::vec3 vPos, glm::vec3 vFront)
{
	position = vPos;
	front = vFront;
	matView = glm::lookAt(position, position + front, up);
}

const glm::mat4& Camera::getLookAt()
{
	return matView;
}

// We essentially change vCamPos using vCamFront and vCamUp
void Camera::ProcessKeyboard(CameraMovement movement, float fDeltaTime)
{
	float fDistance = fCameraSpeed * fDeltaTime;
	switch (movement)
	{
	case CameraMovement::FORWARD:
		position += front * fDistance;
		break;

	case CameraMovement::BACKWARD:
		position -= front * fDistance;
		break;

	case CameraMovement::LEFT:
		position -= glm::normalize(glm::cross(front, up)) * fDistance;
		break;

	case CameraMovement::RIGHT:
		position += glm::normalize(glm::cross(front, up)) * fDistance;
		break;

	case CameraMovement::UP:
		position += up * fDistance;
		break;

	case CameraMovement::DOWN:
		position -= up * fDistance;
		break;
	}

	matView = glm::lookAt(position, position + front, up);
}

// We essentially change vCamDir using pitch and yaw values
void Camera::ProcessMouse(float fMousePosX, float fMousePosY, int screenWidth, int screenHeight, bool bFirstMouse)
{
	if (bFirstMouse)
	{
		fLastX = screenWidth / 2.0f;
		fLastY = screenHeight / 2.0f;
	}

	else
	{
		float fOffsetX = fMousePosX - fLastX;
		float fOffsetY = fLastY - fMousePosY;

		fLastX = fMousePosX;
		fLastY = fMousePosY;

		float fSensitivity = 0.2f;
		fOffsetX *= fSensitivity;
		fOffsetY *= fSensitivity;

		fYaw += fOffsetX;
		fPitch += fOffsetY;

		if (fPitch > 89.0f)
			fPitch = 89.0f;
		else if (fPitch < -89.0f)
			fPitch = -89.0f;

		glm::vec3 vDirection;
		vDirection.x = cosf(glm::radians(fYaw)) * cosf(glm::radians(fPitch));
		vDirection.y = sinf(glm::radians(fPitch));
		vDirection.z = sinf(glm::radians(fYaw)) * cosf(glm::radians(fPitch));
		front = glm::normalize(vDirection);
	}

	matView = glm::lookAt(position, position + front, up);
}

void Camera::SetCameraPos(glm::vec3 vPos)
{
	position = vPos;
}

void Camera::UpdateView(Shader shader, const std::string& viewMat4ID)
{
	shader.use();
	shader.setMat4(viewMat4ID, matView);
}