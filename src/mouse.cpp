#include "mouse.hpp"

Mouse::Mouse(GLFWwindow *window)
{
	this->window = window;
	InitialCursorPosition();
}

void Mouse::InitialCursorPosition()
{
	glfwGetCursorPos(this->window,&this->lastX,&this->lastY);
}

void Mouse::CalculateDelta()
{
	f64 xPos,yPos;
	glfwGetCursorPos(this->window,&xPos,&yPos);

	this->deltaX = static_cast<i32>(xPos - this->lastX);
	this->deltaY = static_cast<i32>(yPos - this->lastY); 

	this->lastX = xPos;
	this->lastY = yPos;
}