#version 430 core

in vec3 outTextureCoordinate;

out vec4 fragmentColor;

uniform samplerCube cubemapSampler;

void main()
{
	fragmentColor = texture(cubemapSampler,outTextureCoordinate);
}