#include "camera.hpp"

Camera::Camera(const glm::vec3 &position, const glm::vec3 &target)
{
	this->position = position;
    this->LookAt(target);
}

void Camera::Move(const glm::vec3 &location)
{
    this->position = location;
}
  
void Camera::LookAt(const glm::vec3 &target)
{
    this->direction = glm::normalize(target - this->position);

    this->left      = glm::normalize(glm::cross(this->direction,glm::vec3(0.0f,1.0f,0.0f)));

    this->up        = glm::cross(this->left,this->direction);
}
