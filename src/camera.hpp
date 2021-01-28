#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.hpp"


struct Camera
{
    glm::vec3 position;
    glm::vec3 direction; // direction the camera is facing
    glm::vec3 left;      
    glm::vec3 up;       
	// vectors direction, left and up form a perpendicular triplet, 
	// where any 2 of the selected vectors are perpendicular to eachother

    Camera(const glm::vec3 &position, const glm::vec3 &target);

    void Move(const glm::vec3 &location);
    void LookAt(const glm::vec3 &target);
};
