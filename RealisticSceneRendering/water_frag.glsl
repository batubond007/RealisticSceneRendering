#version 460 core

uniform sampler2D refractionTex;
uniform sampler2D reflectionTex;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthTexture;
uniform float MoveFactor;

in vec4 clipSpace;
in vec2 texCoords;
in vec3 toEye;
in vec3 fromLight;

out vec4 fragColor;

const float waveStrength = 0.01;
const float shineDamper = 20.0;
const float reflectivity = 0.5;

float near = 1.0;
float far = 10000.0;

void main(void)
{
	vec2 ndc = (clipSpace.xy / clipSpace.w) / 2.0 + 0.5;
	vec2 refractionTexCoords = vec2(ndc.x, ndc.y);
	vec2 reflectionTexCoords = vec2(ndc.x, -ndc.y);

	float depth = texture(depthTexture, refractionTexCoords).r;
	float floorDistance = (2.0 * near * far) / (far + near - (2  * depth  - 1.0) * (far - near));
    
	depth = gl_FragCoord.z;
	float waterDistance = (2.0 * near * far) / (far + near - (2  * depth  - 1.0) * (far - near));
	float waterDepth = (floorDistance - waterDistance) / 50.0;

	vec2 distortedCoords = (texture(dudvMap, vec2(texCoords.x + MoveFactor, texCoords.y))).rg * 0.1;	
	distortedCoords = texCoords + vec2(distortedCoords.x, distortedCoords.y + MoveFactor);
	vec2 distortion = (texture(dudvMap, distortedCoords).rg * 2.0 - 1) * waveStrength * clamp(waterDepth / 3, 0.0, 1.0);

	refractionTexCoords += distortion;
	refractionTexCoords = clamp(refractionTexCoords, 0.001, 0.999);

	reflectionTexCoords += distortion;
	reflectionTexCoords.x = clamp(reflectionTexCoords.x, 0.001, 0.999);
	reflectionTexCoords.y = clamp(reflectionTexCoords.y, -0.999, -0.001);

    vec4 refractionCol = texture(refractionTex, refractionTexCoords);
    vec4 reflectionCol = texture(reflectionTex, reflectionTexCoords);

	// Normal Calculation
	vec4 normalCol = max(texture(normalMap, distortedCoords) - 0.5, 0.0) * 2;
	vec3 normal = vec3(normalCol.r * 2 - 1, normalCol.b * 3, normalCol.g * 2 - 1);
	normal = normalize(normal);

	// Fresnel
	vec3 view = normalize(toEye);
	float refractiveFactor = dot(view, normal);
	refractiveFactor = clamp(pow(refractiveFactor, 0.8), 0.0, 1.0);

	//Specular
	vec3 reflectedLight = reflect(normalize(fromLight), normal);
	float specular = max(dot(reflectedLight, view), 0.0);
	specular = pow(specular, shineDamper);
	vec3 specularHighlights = vec3(1, 226.0/255, 152.0/255) * specular * reflectivity * clamp(waterDepth / 3, 0.0, 1.0);
	
	fragColor = mix(reflectionCol, refractionCol, refractiveFactor);
	fragColor = mix(fragColor, vec4(0, 0.3, 0.5, 1.0), 0.4) + vec4(specularHighlights, 0.0);

	//Smooth Edges
	fragColor.a = clamp(waterDepth / 1.5, 0.0, 1.0);

	//Edge Foam 
	float foam = 1-clamp(waterDepth * 2.5, 0.0, 1.0);
	foam = mix(foam, reflectionTexCoords.r, foam/2);

	vec4 foamFactor = vec4(foam.rrrr) * .8;
	fragColor = clamp(fragColor + foamFactor, 0.0, 1.0);
}