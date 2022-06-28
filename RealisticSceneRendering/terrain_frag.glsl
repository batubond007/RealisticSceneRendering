#version 460 core

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

out vec4 fragColor;

void main(void)
{
    vec4 color = texture(texture_diffuse1, TexCoord);
	fragColor = vec4(color.xyz, 1);
}
