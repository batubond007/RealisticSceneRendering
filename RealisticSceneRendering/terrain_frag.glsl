#version 460 core

in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

out vec4 fragColor;

void main(void)
{
//    vec4 color = texture(texture_diffuse1, TexCoord);
    vec4 color = vec4(TexCoord.xy, 0, 1);
	fragColor = vec4(TexCoord.xy, 1, 1);
}
