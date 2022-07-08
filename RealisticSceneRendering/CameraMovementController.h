#pragma once
#include "SceneObject.h"
#include "Camera.h"
#include "InputController.h"

class CameraMovementController : SceneObject {
	void Start() {
		SceneObject::Start();
	}

	void Update() {
		if (InputController::isMovingForward)
		{
			Camera::cam->ProcessKeyboard(FORWARD, .2f);
		}
		if (InputController::isMovingBackward)
		{
			Camera::cam->ProcessKeyboard(BACKWARD, .2f);
		}
		Camera::cam->ProcessMouseMovement(InputController::xoffset, InputController::yoffset);
	}
};