#version 430 core

in vec2 outTextureCoordinate;

out vec4 fragmentColor;

uniform sampler2D textureSampler;

void main()
{
	fragmentColor = texture(textureSampler,outTextureCoordinate);	
}