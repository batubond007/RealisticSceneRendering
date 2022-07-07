#version 460 core

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;
uniform vec3 eyePos;

out vec4 clipSpace;
out vec2 texCoords;
out vec3 toEye;
out vec3 fromLight;

const float tiling = 4.0;
const vec3 lightPos = vec3(20, 50, 0);

void main(void)
{
    vec4 worldsPos = modelingMatrix * vec4(inVertex, 1);
    clipSpace = projectionMatrix * viewingMatrix * worldsPos;
    texCoords = vec2(inVertex.x, inVertex.z) * tiling;
    toEye = eyePos - worldsPos.xyz;
    fromLight = worldsPos.xyz - lightPos;
    gl_Position = clipSpace;
}

