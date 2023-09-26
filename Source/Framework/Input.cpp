#include "Input.h"

bool Input::GetKey(KeyCode key)
{
	return glfwGetKey(window, (int)key) == GLFW_PRESS;
}

void Input::Initialize(GLFWwindow* window)
{
	Input::window = window;
	glfwSetCursorPosCallback(Input::window, Input::InputMouseCallback);
}

void Input::InputMouseCallback(GLFWwindow* window, double x, double y)
{
	/*lastMousePosition.x = x;
	lastMousePosition.y = y;*/
}