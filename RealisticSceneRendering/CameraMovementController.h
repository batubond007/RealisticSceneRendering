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
		if (InputController::pitchChange != 0)
		{
			Camera::cam->ProcessMouseMovement(InputController::pitchChange, 0);
		}
		if (InputController::yawChange != 0)
		{
			Camera::cam->ProcessMouseMovement(0, InputController::yawChange);
		}
		
	}
};