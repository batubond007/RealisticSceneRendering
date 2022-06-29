#version 460 core

layout (location=0) out vec4 fragColor;

in vec2 uv;

uniform vec3 camPos;
uniform mat4 MVP;

// Transform ray from view to world space
uniform mat4 projView;
uniform mat4 invView;
uniform vec3 sphereCenter;
uniform float innerSphereRadius;
uniform float outerSphereRadius;
uniform float maxRenderDist;
uniform float cloudTopOffset;
uniform float weatherScale;
uniform float baseNoiseScale;
uniform float highFreqNoiseScale;
uniform float highFreqNoiseUVScale;
uniform float highFreqNoiseHScale;


// Noise textures for cloud shapes and erosion
//uniform sampler3D worley;
// weather texture
//uniform sampler2D weather;

// Light data
uniform vec3 lightDir;
uniform vec3 cloudColor;

// Cloud evolution
uniform float cloudType;
uniform float coverageMultiplier;

// Generated Cloud shape noise texture
uniform sampler3D perlinworley;

// used to adjust u coordinate based on aspect ratio
vec2 screenResolution = vec2(1920.0f, 1080.0f);
float FOV = 45.0f;

// Core
void cloudRaymarch(vec3 startPos, vec3 endPos, out vec4 color);
float sampleDensity(vec3 pos, float heightFraction);

// Utility
bool intersectSphere(vec3 o, vec3 d, out vec3 minT, out vec3 maxT);
vec2 sphericalUVProj(vec3 p, vec3 center);
float remap(const float val, const float currMin, const float currMax, const float newMin, const float newMax);
float densityHeightGradient(const float heightFrac);

void main(void)
{
	vec2 fragCoord = gl_FragCoord.xy;
	// computing ray direction based on screen coordinates and camera fov
	vec2 fulluv = fragCoord - screenResolution / 2.0;
	float z =  screenResolution.y / tan(radians(FOV));
	vec3 viewDir = normalize(vec3(fulluv, -z / 2.0));
	vec3 worldDir = normalize( (invView * vec4(viewDir, 0)).xyz);

	// Volume intersection points
	vec3 startPos, endPos;
	bool intersect = intersectSphere(camPos, worldDir, startPos, endPos);
	
	vec4 ambientColor = vec4(0.2, 0.4, 0.69, 1.0);
	if(intersect)
	{
		//fragColor = vec4(textureLod(perlinworley,vec3(uv,0.5f), 0)).rgba;

		vec4 color = vec4(0.0f);
		cloudRaymarch(startPos, endPos, color);

		fragColor = color;
		// TODO: Ray march!
		/*
		// If intersected, raymarch cloud
		vec3 outColor = vec3(0);
		float density = frontToBackRaymarch(startPos, endPos, outColor);
		density = clamp(density, 0.0, 1.0);

		vec4 finalColor = vec4(outColor, density);
		float alpha = length(startPos - camPos) / maxRenderDist;
		alpha *= alpha;
		alpha = clamp(alpha, 0, 1);
		finalColor = mix(finalColor, ambientColor * lightFactor, alpha);
		fragColor = finalColor;*/
	}
	else
	{
		fragColor = ambientColor;
	}
}

vec2 sphericalUVProj(vec3 p, vec3 center)
{
	vec3 dirVector = normalize(p - center); // [-1,1] range
	return (dirVector.xz + 1.0) / 2.0; // remap to [0,1] range for uv
}

int marchStepCount = 12;
void cloudRaymarch(vec3 startPos, vec3 endPos, out vec4 color)
{
	vec3 path = endPos - startPos;
	vec3 dir = normalize(path);

	float pathLen = sqrt(dot(path, path));
	float stepSize = pathLen / marchStepCount;

	vec3 currPos = startPos;

	vec4 finalColor = vec4(0.0);
	float sampledDensity = 0.0f;


	for(int i = 0 ; i < marchStepCount; i++)
	{
		// sample density at currPos, fill in alpha val
		float heightFraction = clamp(currPos.y / (endPos.y - startPos.y), 0.0f, 1.0f);
		
		sampledDensity = sampleDensity(currPos, heightFraction);
		if(sampledDensity > 0.01)
		{
			// TODO calculate color via light raymarching
			vec3 sampledColor = vec3(0.55, 0.7, 0.7);
			vec4 c = vec4(sampledColor * finalColor.w, sampledDensity);

			//finalColor.xyz += c.xyz;
			//finalColor.w = (1.0 - sampledDensity) * finalColor.w;

			finalColor = (1.0 - finalColor.a) * c + finalColor;

			if(finalColor.a >= 0.95) // EARLY EXIT ON FULL OPACITY
				break;

		}
		if(1.0 - finalColor.w > 0.95f)
		{
			// no point sampling after reaching full opaque, early exit.
			//break;
		}

		currPos += dir * stepSize;
	}
	//finalColor.w = 1.0 - finalColor.w;
	//color = vec4(finalColor.xyz * finalColor.w, finalColor.w);

	// Return alpha (density) and store final color
	color = finalColor;
}

float sampleDensity(vec3 pos, float heightFraction)
{
	vec2 uv = sphericalUVProj(pos, sphereCenter);
	vec4 baseNoise = texture(perlinworley, vec3(uv, heightFraction));
	// Build the low frequency fbm modifier
	float lowFreqFBM = (baseNoise.g * 0.625) + (baseNoise.b * 0.25) + (baseNoise.a * 0.125);
	float baseShape = remap(baseNoise.r, -(1.0 - lowFreqFBM), 1.0, 0.0, 1.0);

	return baseShape;
}
// ==========================================================================
// Find Raymarch start-end point by intersecting with atmosphere lower&upper bound spheres
bool intersectSphere(vec3 o, vec3 d, out vec3 minT, out vec3 maxT)
{
	// Intersect inner sphere
	vec3 sphereToOrigin = (o - sphereCenter);
	float b = dot(d, sphereToOrigin);
	float c = dot(sphereToOrigin, sphereToOrigin);
	float sqrtOpInner = b*b - (c - innerSphereRadius * innerSphereRadius);

	// No solution (we are outside the sphere, looking away from it)
	float maxSInner;
	if(sqrtOpInner < 0.0)
	{
		return false;
	}
	
	float deInner = sqrt(sqrtOpInner);
	float solAInner = -b - deInner;
	float solBInner = -b + deInner;

	maxSInner = max(solAInner, solBInner);

	if(maxSInner < 0.0) return false;

	maxSInner = maxSInner < 0.0? 0.0 : maxSInner;
	
	// Intersect outer sphere
	float sqrtOpOuter = b*b - (c - outerSphereRadius * outerSphereRadius);

	// No solution - same as inner sphere
	if(sqrtOpOuter < 0.0)
	{
		return false;
	}
	
	float deOuter = sqrt(sqrtOpOuter);
	float solAOuter = -b - deOuter;
	float solBOuter = -b + deOuter;

	float maxSOuter = max(solAOuter, solBOuter);

	if(maxSOuter < 0.0) return false;

	maxSOuter = maxSOuter < 0.0? 0.0 : maxSOuter;

	// Compute entering and exiting ray points
	float minSol = min(maxSInner, maxSOuter);

	if(minSol > maxRenderDist)
	{
		return false;
	}

	float maxSol = max(maxSInner, maxSOuter);

	minT = o + d * minSol;
	maxT = o + d * maxSol;

	return true;
}

// Utility function that maps a value from one range to another. SIGGRAPH 2017 Nubis-Decima
float remap(const float val, const float currMin, const float currMax, const float newMin, const float newMax)
{
	float currFrac = ((val - currMin) / (currMax - currMin));
	return newMin + (currFrac * (newMax - newMin));
}

float densityHeightGradient(const float heightFrac) {

	vec4 cloudGradient = vec4(0.02f, 0.4f, 0.68f, 0.925f);
	return smoothstep(cloudGradient.x, cloudGradient.y, heightFrac) - smoothstep(cloudGradient.z, cloudGradient.w, heightFrac);
}
