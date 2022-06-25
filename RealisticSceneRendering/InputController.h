#pragma once
#include "Window.h"

class InputController {
public:
	float Speed;

    InputController() {
        Speed = 5;
        Window::windowObj->SetKeyCallback(keyCallbackStatic);
    }

private:
    static void keyCallbackStatic(GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods);
};