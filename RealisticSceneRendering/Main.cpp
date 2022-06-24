#include <iostream>
#include <vector>

#include "SceneObject.h"
#include "Window.h"

using namespace std;

class Runner {
public:
	void run() {
		Initialize();
		Start();
		while (!window->WindowShouldClose())
		{
			Update();
			window->SwapBuffers();
			window->PollEvents();
		}
	}
private:
	Window *window;

	void InitializeWindow() {
		window = new Window(1920, 1080, "Realistic Scene Rendering");
		if (window->Initialize() == EXIT_FAILURE)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
		}
		window->reshape(window->window, 1920, 1080);
	}

	void Initialize() {
		InitializeWindow();
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

int main() {
	Runner runner;
	runner.run();
}
