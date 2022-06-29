#version 460 core

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inUV;

out vec2 uv;

uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    //gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
	gl_Position = vec4(inVertex,1);
    //gl_Position = vec4(inVertex, 1);
    uv = inUV;
}

