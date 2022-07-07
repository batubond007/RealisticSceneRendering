#version 460 core

uniform sampler2D refractionTex;
uniform sampler2D reflectionTex;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float MoveFactor;

in vec4 clipSpace;
in vec2 texCoords;
in vec3 toEye;
in vec3 fromLight;

out vec4 fragColor;

const float waveStrength = 0.01;

void main(void)
{
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y);

	vec2 distortedCoords = (texture(dudvMap, vec2(texCoords.x + MoveFactor, texCoords.y))).rg * 0.1;	
	distortedCoords = texCoords + vec2(distortedCoords.x, distortedCoords.y + MoveFactor);
	vec2 distortion = (texture(dudvMap, distortedCoords).rg * 2.0 - 1) * waveStrength;

	refractionTexCoords += distortion;
	refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);

	reflectionTexCoords += distortion;
	reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
	reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.999, -0.001);


    vec4 refractionCol = texture(refractionTex, refractionTexCoords);
    vec4 reflectionCol = texture(reflectionTex, reflectionTexCoords);

	vec4 normalCol = texture(refractionTex, texCoords);
	vec3 normal = vec3(normalCol.r * 2 - 1, normalCol.b, normalCol.g * 2 - 1);
	normal = normalize(normal);

	vec3 view = normalize(toEye);
	float refractiveFactor = dot(view, vec3(0,1,0));
	refractiveFactor = pow(refractiveFactor, 2);
	
	fragColor = mix(reflectionCol, refractionCol, refractiveFactor);
	fragColor = mix(fragColor, vec4(0, 0.3, 0.5, 1.0), 0.2);
}