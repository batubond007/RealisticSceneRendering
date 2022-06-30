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

private:
	Mesh* quad;
	Shader* cloudShader;
	Mesh* createQuad();
	GLuint worleyTex, perlinTex, weatherTex;
	GLuint perlinWorleyComputeID;

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

	GLuint uInvView;
	GLuint uProjView;
};