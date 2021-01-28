#version 430 core

layout (location = 0) in vec3 inPosition;

out vec3 outTextureCoordinate;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = (projection * view * vec4(inPosition,1.f)).xyww;
	outTextureCoordinate = inPosition;
}