#pragma once

#include <iostream>
#include <vector>

#include "SceneObject.h"
#include "Window.h"
#include "WaterRenderer.h"
#include "Camera.h"
#include "InputController.h"

using namespace std;

class Runner {
public:
	void run() {
		Initialize();
		Start();
		while (!Window::windowObj->WindowShouldClose())
		{
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
		InputController inputController;
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
};