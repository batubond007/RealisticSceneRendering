#pragma once
#include "Window.h"

class InputController {
public:
    static bool isMovingForward;
    static bool isMovingBackward;
	static float yawChange;
	static float pitchChange;
	static float speed;

    static void SetCallbacks();

private:
    static float lastX, lastY;
    static void keyCallbackStatic(GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods);
    static void mouseCallbackStatic(GLFWwindow* window, double xpos, double ypos);
};