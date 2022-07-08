#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include "Camera.h"

class VolumetricClouds : public SceneObject
{
public:

	void Start();
	void Update();
	void generateNoiseTextures();

	unsigned int generateTexture3D(int w, int h, int d);
	void SetUniforms();
	void CacheUniformLocations();

	static float evolveClouds;
	static float coverageController;

private:
	Mesh* quad;
	Shader* cloudShader;
	Mesh* createQuad();
	GLuint worleyTex, perlinTex, weatherTex;
	GLuint perlinWorleyComputeID, worleyComputeID;

	// uniform locations
	GLuint uSphereCenter;
	GLuint uInnerSphereRadius;
	GLuint uOuterSphereRadius;
	GLuint uMaxDrawDistance;
	GLuint uTopOffset;
	GLuint uWeatherScale;
	GLuint uBaseNoiseScale;
	GLuint uHighFreqNoiseScale;
	GLuint uHighFreqNoiseUVScale;
	GLuint uHighFreqNoiseHScale;
	GLuint uLightDir;
	GLuint uCloudColor;
	GLuint uCloudType;
	GLuint uCoverageMultiplier;
	GLuint uLightColor;
	GLuint uLightFactor;
	GLuint uZenitColor;
	GLuint uHorizonColor;
	GLuint uInvView;
	GLuint uProjView;
	GLuint uTime;
	GLuint uRotation;
	GLuint uEvolveClouds;
};