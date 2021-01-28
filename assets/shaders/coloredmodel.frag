#version 430 core

in vec3 outColorNormal;
//in vec3 outColor;
//in vec3 outNormal;

out vec4 fragmentColor;

void main()
{
	//fragmentColor = vec4(outColor,0.0);
	fragmentColor = vec4(abs(outColorNormal),0.0);
}