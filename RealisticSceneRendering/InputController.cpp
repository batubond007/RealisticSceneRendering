#include "InputController.h"
#include "Camera.h"

bool InputController::isMovingForward;
bool InputController::isMovingBackward;
float InputController::yawChange;
float InputController::pitchChange;
float InputController::speed = 5;

float InputController::lastX;
float InputController::lastY;

float dist;


void InputController::SetCallbacks() {

    Window::windowObj->SetKeyCallback(InputController::keyCallbackStatic);
    Window::windowObj->SetMouseCallback(InputController::mouseCallbackStatic);
}

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
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        if (!isMovingForward)
        {
            isMovingForward = true;
            isMovingBackward = false;
        }
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        if (!isMovingBackward)
        {
            isMovingForward = false;
            isMovingBackward = true;
        }
    }
    if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        if (isMovingForward)
        {
            isMovingForward = false;
        }
    }
    if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        if (isMovingBackward)
        {
            isMovingBackward = false;
        }
    }
}

void InputController::mouseCallbackStatic(GLFWwindow* window, double xpos, double ypos) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS)
    {
        InputController::yawChange = xpos - lastX;
        InputController::pitchChange = ypos - lastY;
    }
    if (state == GLFW_RELEASE)
    {
        InputController::yawChange = 0;
        InputController::pitchChange = 0;
    }
    InputController::lastX = xpos;
    InputController::lastY = ypos;
}
