#version 460 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

out vec4 fragColor;
const vec3 lightPos = vec3(10000, 50000, 10000);

void main(void)
{
	vec4 color = texture(texture_diffuse1, TexCoord);
	// vec4 color = vec4(TexCoord.xy, 0, 1);
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1, 226.0/255, 152.0/255);

	fragColor = vec4(color.xyz * clamp(diffuse + vec3(.5, .5, .5), 0.0, 1.0), 1);
}
