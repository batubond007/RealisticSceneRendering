#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <iostream>

class Window 
{
public:
	static int width;
	static int height;
	static int major;
	static int minor;
	const char* windowName;
	GLFWwindow* window;

	// Constructs the window
	Window(int width, int height, const char * windowName, int major = 4, int minor = 6) {
		this->width = width;
		this->height = height;
		this->windowName = windowName;
		this->major = major;
		this->minor = minor;
		window = nullptr;
	}

	// Initializes the window
	int Initialize() {
		if (!glfwInit())
		{
			exit(-1);
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

		if (!window)
		{
			glfwTerminate();
			exit(-1);
		}

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		if (GLEW_OK != glewInit())
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return EXIT_FAILURE;
		}

		char rendererInfo[512] = { 0 };
		strcpy_s(rendererInfo, (const char*)glGetString(GL_RENDERER));
		strcat_s(rendererInfo, " - ");
		strcat_s(rendererInfo, (const char*)glGetString(GL_VERSION));
		glfwSetWindowTitle(window, rendererInfo);

		glfwSetWindowSizeCallback(window, reshape);
	}

	// Terminates the window
	void Terminate() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	// Sets Keyboard callback
	void SetKeyCallback(GLFWkeyfun keyboard) {
		glfwSetKeyCallback(window, keyboard);
	}

	// Checks if window is closed
	bool WindowShouldClose() {
		return glfwWindowShouldClose(window);
	}

	// Swap window buffers
	void SwapBuffers() {
		glfwSwapBuffers(window);
	}

	// Gets the events
	void PollEvents() {
		glfwPollEvents();
	}

	static void reshape(GLFWwindow* window, int w, int h)
	{
		w = w < 1 ? 1 : w;
		h = h < 1 ? 1 : h;

		width = w;
		height = h;

		glViewport(0, 0, w, h);
	}
};

#endif