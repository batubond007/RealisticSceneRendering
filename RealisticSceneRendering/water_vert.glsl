#version 460 core

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;


uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}

