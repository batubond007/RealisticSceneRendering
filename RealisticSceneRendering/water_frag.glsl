#version 460 core

in vec4 col;
out vec4 fragColor;

void main(void)
{
	fragColor = vec4(col.xy, 1, 1);
//	fragColor = vec4(0, 0, 1, 1);
}
