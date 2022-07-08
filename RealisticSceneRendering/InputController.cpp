#include "InputController.h"
#include "Camera.h"
#include "VolumetricClouds.h"

bool InputController::isMovingForward;
bool InputController::isMovingBackward;
float InputController::xoffset;
float InputController::yoffset;
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
    if (key == GLFW_KEY_L && action == GLFW_RELEASE)
    {
        if (VolumetricClouds::evolveClouds > 0.9) {

            VolumetricClouds::evolveClouds = 0.0f;
        }
        else {
            VolumetricClouds::evolveClouds = 1.0f;
        }
    }
    if (key == GLFW_KEY_C && action == GLFW_RELEASE)
    {
        if (VolumetricClouds::coverageController < 1.0) {

            VolumetricClouds::coverageController += 0.05f;
        }
        else {
            VolumetricClouds::coverageController = 0.3f;

        }
    }
}

void InputController::mouseCallbackStatic(GLFWwindow* window, double xpos, double ypos) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS)
    {
        InputController::xoffset = xpos - lastX;
        InputController::yoffset = ypos - lastY;
    }
    else if(state == GLFW_RELEASE) {
        InputController::xoffset = 0;
        InputController::yoffset = 0;
    }
    InputController::lastX = xpos;
    InputController::lastY = ypos;


}
