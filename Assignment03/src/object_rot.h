#ifndef _OBJECT_ROT_H_
#define _OBJECT_ROT_H_

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef class __ObjectRot {
public:
	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void mouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
	}

	const float yaw() { return glm::radians(Yaw); }
	const float pitch() { return glm::radians(-Pitch); }
	void setYaw(float y) { Yaw = y; }
	void setPitch(float p) { Pitch = p; }

	__ObjectRot() {
		Yaw = 0.0f;
		Pitch = 0.0f;
		MouseSensitivity = 0.2f;
	}
private:
	// euler Angles
	float Yaw;
	float Pitch;

	// camera options
	float MouseSensitivity;
} ObjectRot;


#endif