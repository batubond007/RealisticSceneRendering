#include "InputController.h"

void InputController::keyCallbackStatic(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}