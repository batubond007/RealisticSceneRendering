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
		for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
		{
			SceneObject::sceneObjects[i]->Start();
		}
	}

	void Update() {
		for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
		{
			SceneObject::sceneObjects[i]->Update();
		}
	}

	void ClearBits() {
		glClearColor(0, 0, 0, 1);
		glClearDepth(1.0f);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
};