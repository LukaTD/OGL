#version 430 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColorNormal; // use vertex normals as colors 
//layout (location = 1) in vec3 inColor;
//layout (location = 2) in vec3 inNormal;

out vec3 outColorNormal;
//out vec3 outColor;
//out vec3 outNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(inPosition,1.0);
	outColorNormal = inColorNormal;
	//outColor = inColor;
	//outNormal = inNormal;
}