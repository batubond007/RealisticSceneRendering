#version 460 core

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

uniform vec4 clipPlane;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main(void)
{
    vec4 worldPos = modelingMatrix * vec4(inVertex, 1);
    
    gl_ClipDistance[0] = dot(worldPos, clipPlane);
    
    Normal = inNormal;
    TexCoord = inTexCoord;
    gl_Position = projectionMatrix * viewingMatrix * worldPos;
    FragPos = gl_Position.xyz;
}

