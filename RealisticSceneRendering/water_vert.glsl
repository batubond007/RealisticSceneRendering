#version 460 core

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;


uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

uniform sampler2D refractionTex;

out vec4 col;

void main(void)
{
    col = texture(refractionTex, inVertex.xy);
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}

