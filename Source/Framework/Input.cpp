#include "Input.h"

bool Input::GetKey(KeyCode key)
{
	return glfwGetKey(window, (int)key) == GLFW_PRESS;
}

bool Input::GetMouseButton(MouseCode button)
{
	return glfwGetMouseButton(window, (int)button) == GLFW_PRESS;
}

int Input::GetMouseX()
{
	return lastMouseX;
}

int Input::GetMouseY()
{
	return lastMouseY;
}

void Input::Initialize(GLFWwindow* window)
{
	Input::window = window;
	glfwSetCursorPosCallback(Input::window, Input::InputMouseCallback);
}

void Input::InputMouseCallback(GLFWwindow* window, double x, double y)
{
	lastMouseX = x;
	lastMouseY = y;
}