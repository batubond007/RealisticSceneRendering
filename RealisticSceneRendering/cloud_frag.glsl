#version 460 core

// Marching algorithms are taken from https://github.com/NadirRoGue/RenderEngine/tree/master/RenderEngine,
// special thanks to NadirRoGue.

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
// ambient & sky lighting
uniform vec3 lightDir;
uniform vec3 realLightColor;
uniform float lightFactor;
uniform vec3 zenitColor;
uniform vec3 horizonColor;
uniform vec3 cloudColor;

// Cloud evolution
uniform float cloudType;
uniform float coverageMultiplier;

// Generated Cloud shape noise texture
uniform sampler3D perlinworley;
uniform sampler3D worley;

// used to adjust uv
vec2 screenResolution = vec2(1920.0f, 1080.0f);
float FOV = 45.0f;

// Cloud types height density gradients
#define STRATUS_GRADIENT vec4(0.0, 0.1, 0.2, 0.3)
#define STRATOCUMULUS_GRADIENT vec4(0.02, 0.2, 0.48, 0.625)
#define CUMULUS_GRADIENT vec4(0.00, 0.1625, 0.88, 0.98)

// Light sampling constants
#define CONE_STEP 0.1666666
// Cone sampling random offsets
uniform vec3 noiseKernel[6u] = vec3[] 
(
	vec3( 0.38051305,  0.92453449, -0.02111345),
	vec3(-0.50625799, -0.03590792, -0.86163418),
	vec3(-0.32509218, -0.94557439,  0.01428793),
	vec3( 0.09026238, -0.27376545,  0.95755165),
	vec3( 0.28128598,  0.42443639, -0.86065785),
	vec3(-0.16852403,  0.14748697,  0.97460106)
);

// Core functions
void cloudRaymarch(vec3 startPos, vec3 endPos, out vec4 color);
float sampleDensity(vec3 pos, float currTotalDensity, float heightFraction, float lod);

// Utility
bool intersectSphere(vec3 o, vec3 d, out vec3 minT, out vec3 maxT);
vec2 sphericalUVProj(vec3 p, vec3 center);
float remap(const float val, const float currMin, const float currMax, const float newMin, const float newMax);
float getDensityForCloud(float heightFraction, float cloudType);

// Lighting
float henyeyGreenstein(vec3 l, vec3 v, float g, float ca);
float beer(float density);
// Beer's law inverted equation for edges
float powder(float density, float ca);
float lightEnergy(vec3 l, vec3 v, float ca, float coneDensity);
vec3 ambientLight();

// Ray march configs
int marchStepCount = 64;
int lightMarchStepCount = 6;

// Random offset added to starting ray depth to prevent banding artifacts 
#define BAYER_FACTOR 1.0/16.0
uniform float bayerFilter[16u] = float[]
(
	0.0*BAYER_FACTOR, 8.0*BAYER_FACTOR, 2.0*BAYER_FACTOR, 10.0*BAYER_FACTOR,
	12.0*BAYER_FACTOR, 4.0*BAYER_FACTOR, 14.0*BAYER_FACTOR, 6.0*BAYER_FACTOR,
	3.0*BAYER_FACTOR, 11.0*BAYER_FACTOR, 1.0*BAYER_FACTOR, 9.0*BAYER_FACTOR,
	15.0*BAYER_FACTOR, 7.0*BAYER_FACTOR, 13.0*BAYER_FACTOR, 5.0*BAYER_FACTOR
);


void main(void)
{
	vec2 fragCoord = gl_FragCoord.xy;
	// computing ray direction based on screen coordinates and camera fov
	vec2 fulluv = fragCoord - screenResolution / 2.0;
	
	float z =  screenResolution.y / tan(radians(FOV));
	vec3 viewDir = normalize(vec3(fulluv, -z / 2.0));
	vec3 worldDir = normalize((invView * vec4(viewDir, 0)).xyz);
	
	// Volume intersection points
	vec3 startPos, endPos;
	bool intersect = intersectSphere(camPos, worldDir, startPos, endPos);
	
	//vec4 ambientColor = vec4(0.2, 0.4, 0.69, 1.0);
	vec4 ambientColor = vec4(ambientLight(), 0.6f);
	
	vec4 color = vec4(0.0f);
	if(intersect)
	{
		//fragColor = vec4(textureLod(perlinworley,vec3(uv,0.5f), 0)).rgba;
		cloudRaymarch(startPos, endPos, color);

		color.a = clamp(color.a, 0.0, 1.0);

		float alpha = length(startPos - camPos) / maxRenderDist;
		alpha *= alpha;
		alpha = clamp(alpha, 0.0, 1.0);
		fragColor = mix(color, ambientColor * lightFactor, alpha);
	}
	else
	{
		fragColor = ambientColor * lightFactor;
	}
}

vec2 sphericalUVProj(vec3 p, vec3 center)
{
	vec3 dirVector = normalize(p - center); // [-1,1] range
	return (dirVector.xz + 1.0) / 2.0; // remap to [0,1] range for uv
}
// Retrieves the cloud density based on cloud type and weighting between default cloud models
float getDensityForCloud(float heightFraction, float cloudType)
{
	float stratusFactor = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
	float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
	float cumulusFactor = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;

	vec4 baseGradient = stratusFactor * STRATUS_GRADIENT + stratoCumulusFactor * STRATOCUMULUS_GRADIENT + cumulusFactor * CUMULUS_GRADIENT;

	// gradicent computation (see Siggraph 2017 Nubis-Decima talk)
	float result = remap(heightFraction, baseGradient.x, baseGradient.y, 0.0, 1.0) * remap(heightFraction, baseGradient.z, baseGradient.w, 1.0, 0.0);
	return result;
}

float getHeightFraction(vec3 p)
{
	return (length(p - sphereCenter) - innerSphereRadius) / (outerSphereRadius - innerSphereRadius);
}
// ==========================================================================
// Lighting functions
// Scattering phase function
float henyeyGreenstein(vec3 l, vec3 v, float g, float ca)
{
	float g2 = g * g;

	return ((1.0 - g2) / pow((1.0 + g2 - 2.0 * g * ca), 1.5 )) * (1.0 / (4.0 * 3.1415));
}

// Intensity decreases with density
float beer(float density)
{
	return exp(-density);
}

// Beer's law inverted equation for edges
float powder(float density, float ca)
{
	float f = 1.0 - exp(-density * 2.0);
	return mix(1.0, f, clamp(-ca * 0.5 + 0.5, 0.0, 1.0));
}

// Full cloud light energy equation
float lightEnergy(vec3 l, vec3 v, float ca, float coneDensity)
{
	return 15 * beer(coneDensity) * powder(coneDensity, ca) * henyeyGreenstein(l, v, 0.2, ca);
}

// Returns an ambient lighting depending on the height
vec3 ambientLight()
{
	vec3 ambientColor = mix(horizonColor, zenitColor, 0.15);
	return mix(realLightColor, ambientColor, 0.65) * lightFactor * 1.85;
}

float raymarchToLight(vec3 pos, vec3 dir, float stepSize)
{
	vec3 startPos = pos;
	// Modify step size to take as much info as possible in 6 steps
	vec3 rayStep = lightDir * (stepSize) * 0.7;
	// Starting cone radius. Will increase as we move along it
	float coneRadius = 1.0;
	// Sampled density until now
	float invDepth = 1.0 / (stepSize * lightMarchStepCount);
	float density = 0.0;
	// Density - transmittance until now
	float coneDensity = 0.0;

	for(int i = 0; i < lightMarchStepCount; i++)
	{
		// Get position inside cone
		vec3 posInCone = startPos + lightDir + coneRadius * noiseKernel[i % 6] * i;

		// By advancing towards the light we might go outside the atmosphere
		float heightFraction = getHeightFraction(posInCone);
		if(heightFraction <= 1.0)
		{
			float lod = mix(0.0, 6.0, float (i)/ lightMarchStepCount);
			float cloudDensity = sampleDensity(posInCone,coneDensity, heightFraction, lod);
			if(cloudDensity > 0.01)
			{
				density += cloudDensity;
				float transmittance = 1.0 - (density * invDepth);
				coneDensity += (cloudDensity * transmittance);
			}
		}

		startPos += rayStep;
		coneRadius += CONE_STEP;
	}

	float ca = dot(lightDir, dir);

	// Compute light energy arriving at point
	return lightEnergy(lightDir, dir, ca, coneDensity);
}

void cloudRaymarch(vec3 startPos, vec3 endPos, out vec4 color)
{
	vec3 path = endPos - startPos;
	vec3 dir = normalize(path);

	float pathLen = sqrt(dot(path, path));
	float stepSize = pathLen / marchStepCount;

	vec3 currPos = startPos;

	vec4 finalColor = vec4(0.0);
	float sampledDensity = 0.0f;
	
	// ambient lighting attenuation factor
	float ambientFactor =  max(min(lightFactor, 1.0), 0.1);
	// full lighting
	vec3 light = realLightColor * lightFactor * cloudColor;
	vec3 ambientL = ambientLight();

	float lodAlpha = clamp(length(startPos - camPos) / maxRenderDist, 0.0, 1.0);
	float samplingLod = mix(0.0, 6.0, lodAlpha);

	// Dithering on the starting ray position to reduce banding artifacts
	int a = int(gl_FragCoord.x) % 4;
	int b = int(gl_FragCoord.y) % 4;
	currPos += dir * bayerFilter[a * 4 + b];

	for(int i = 0 ; i < marchStepCount; i++)
	{
		// sample density at currPos, fill in alpha val
		//float heightFraction = clamp(length(currPos / (endPos - startPos)), 0.0f, 1.0f);
		float heightFraction = getHeightFraction(currPos);

		sampledDensity = sampleDensity(currPos,finalColor.a, heightFraction, samplingLod);
		if(sampledDensity > 0.001)
		{
			// TODO calculate color via light raymarching
			//vec3 sampledColor = vec3(0.90, 0.95, 0.1);

			float lightEnergy = raymarchToLight(currPos, dir, stepSize);

			vec3 sampledColor = vec3(light * lightEnergy + ambientL); 
			vec4 c = vec4(sampledColor * (sampledDensity), sampledDensity);
			finalColor += (1.0 - finalColor.a) * c ;
		}
		if(finalColor.a >= 0.95) // EARLY EXIT ON FULL OPACITY
			break;
		currPos += dir * stepSize;
	}
	color = finalColor;
}

float sampleDensity(vec3 pos, float totalDensity, float heightFraction, float lod)
{
	// Make clouds evolve with wind
	//pos += heightFraction * windDirection * cloudTopOffset;
	//pos += windDirection * time * cloudSpeed;

	vec2 uv = sphericalUVProj(pos, sphereCenter);

	// Sample base cloud shape noises (Perlin-Worley + 3 Worley)
	vec4 baseNoise = textureLod(perlinworley, vec3(uv, heightFraction), lod);
	// Build the low frequency fbm modifier
	float lowFreqFBM = (baseNoise.g * 0.625) + (baseNoise.b * 0.25) + (baseNoise.a * 0.125);
	float baseShape = remap(baseNoise.r, -(1.0 - lowFreqFBM), 1.0, 0.0, 1.0);

	
	// Apply density gradient based on cloud type
	//float densityGradient = getDensityForCloud(heightFraction, cloudType);

	baseShape *= getDensityForCloud(heightFraction, heightFraction * 0.7);
	float coverage = 0.25;
	//float coverage = 0.5f;
	// Make sure cloud with less density than actual coverage dissapears
	float coveragedCloud = remap(baseShape, coverage, 1.0, 0.0, 1.0);
	coveragedCloud *= coverage;
	coveragedCloud *= mix(1.0, 0.0, clamp(heightFraction / coverage, 0.0, 1.0));

	float finalCloud = coveragedCloud;
	
	// Only erode the cloud if erosion will be visible (low density sampled until now)
	if(totalDensity < 0.1)
	{
		// Build−high frequency Worley noise FBM.
		vec3 erodeCloudNoise = textureLod(worley, vec3(uv * 1.3, heightFraction * 0.2) * 1, lod).rgb;
		float highFreqFBM = (erodeCloudNoise.r * 0.625) + (erodeCloudNoise.g * 0.25) + (erodeCloudNoise.b * 0.125);

		// Recompute height fraction after applying wind and top offset
		// heightFraction = getHeightFraction(pos);

		float highFreqNoiseModifier = mix(highFreqFBM, 1.0 - highFreqFBM, clamp(heightFraction * 8.5, 0.0, 1.0));
		//coveragedCloud = coveragedCloud - highFreqNoiseModifier * (1.0 - coveragedCloud);
		finalCloud = remap(coveragedCloud, highFreqNoiseModifier * 0.45, 1.0, 0.0, 1.0);
	}
	return clamp(finalCloud, 0.0, 1.0);
}


// Utility function that maps a value from one range to another. SIGGRAPH 2017 Nubis-Decima
float remap(const float val, const float currMin, const float currMax, const float newMin, const float newMax)
{
	float currFrac = ((val - currMin) / (currMax - currMin));
	return newMin + (currFrac * (newMax - newMin));
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