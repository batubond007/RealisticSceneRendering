#pragma once

#include <iostream>
#include <vector>

#include "SceneObject.h"
#include "Window.h"
#include "Camera.h"
#include "InputController.h"
#include "CameraMovementController.h"

using namespace std;

class Runner {
public:
	void run() {
		Initialize();
		Start();
		while (!Window::windowObj->WindowShouldClose())
		{
			ClearBits();
			Update();
			Window::windowObj->SwapBuffers();
			Window::windowObj->PollEvents();
		}
	}
	static void DrawScene();
	static void DrawSceneWOWater();
	static void ClearBits();
private:
	void InitializeWindow() {
		Window::windowObj = new Window(1920, 1080, "Realistic Scene Rendering");
		if (Window::windowObj->Initialize() == EXIT_FAILURE)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
		}
		Window::windowObj->reshape(Window::windowObj->window, 1920, 1080);
	}

	void Initialize() {
		InitializeWindow();
		Camera::cam = new Camera();
		InputController::SetCallbacks();
		CameraMovementController* cameraController = new CameraMovementController();
	}

	void Start() {
		glEnable(GL_DEPTH_TEST);
		for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
		{
			SceneObject::sceneObjects[i]->Start();
		}
	}

	void Update() {
		DrawScene();
	}
};