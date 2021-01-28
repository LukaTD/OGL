#pragma once

#include <iostream>

#include <GLFW/glfw3.h>

#include "types.hpp"

struct Mouse
{
	GLFWwindow *window;
	f64 lastX;
	f64 lastY;
	i32 deltaX;
	i32 deltaY;
	bool mouseOn = false;

	Mouse(GLFWwindow *window);
	void InitialCursorPosition();
	void CalculateDelta();
};