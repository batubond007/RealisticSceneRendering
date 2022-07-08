#include "VolumetricClouds.h"
#include "Runner.h"

float VolumetricClouds::evolveClouds = 0.0f;
float VolumetricClouds::coverageController = 0.0f;

void VolumetricClouds::Start() {
	SceneObject::Start();
	quad = createQuad();
	cloudShader = new Shader("cloud_vert.glsl", "cloud_frag.glsl",nullptr);
	cloudShader->setVec3("camPos", Camera::cam->Position);

	CacheUniformLocations();
	generateNoiseTextures();
}
void VolumetricClouds::generateNoiseTextures()
{
	Shader* perlinworleyComp = new Shader("perlinWorleyGen.comp");

	int res = 128;
	this->perlinTex = generateTexture3D(res, res, res);
	perlinworleyComp->use();

	std::cout << "computing perlinworley!" << std::endl;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	glBindImageTexture(0, this->perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	glGenerateMipmap(GL_TEXTURE_3D);
	perlinworleyComp->setInt("outNoise", 0);

	glDispatchCompute(res, res, res);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	this->perlinWorleyComputeID = perlinworleyComp->ID;
	std::cout << "computed perlinworley" << std::endl;

	res = 4;
	Shader* worleyCompute = new Shader("worley.comp");
	this->worleyTex = generateTexture3D(res, res, res);
	worleyCompute->use();

	std::cout << "computing Worley" << std::endl;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->worleyTex);
	glBindImageTexture(0, this->worleyTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	glGenerateMipmap(GL_TEXTURE_3D);

	worleyCompute->setInt("outNoise", 0);

	glDispatchCompute(res, res, res);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_3D, this->worleyTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	this->worleyComputeID = worleyCompute->ID;
	std::cout << "computed Worley" << std::endl;
}


void VolumetricClouds::Update() {
	SceneObject::Update();

	cloudShader->use();
	cloudShader->setMat4("projectionMatrix", Camera::cam->GetProjectionMatrix());
	cloudShader->setMat4("viewingMatrix", Camera::cam->GetViewMatrix());
	cloudShader->setMat4("MVP", Camera::cam->GetProjectionMatrix() * Camera::cam->GetViewMatrix());
	

	SetUniforms();
	cloudShader->setInt("perlinworley",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->perlinTex);

	/*
	cloudShader->setInt("worley", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, this->worleyTex);*/

	glDepthMask(GL_FALSE);
	quad->Draw(*cloudShader);
	glDepthMask(GL_TRUE);
}

void VolumetricClouds::CacheUniformLocations()
{
	GLuint programID = cloudShader->ID;
	uSphereCenter = glGetUniformLocation(programID, "sphereCenter");
	uInnerSphereRadius = glGetUniformLocation(programID, "innerSphereRadius");
	uOuterSphereRadius = glGetUniformLocation(programID, "outerSphereRadius");
	uLightDir = glGetUniformLocation(programID, "lightDir");
	uCoverageMultiplier = glGetUniformLocation(programID, "coverageMultiplier");

	uMaxDrawDistance = glGetUniformLocation(programID, "maxRenderDist");

	uProjView = glGetUniformLocation(programID, "projView");
	uInvView = glGetUniformLocation(programID, "invView");
	uTime = glGetUniformLocation(programID, "time");
	uRotation = glGetUniformLocation(programID, "rotation");
	uEvolveClouds = glGetUniformLocation(programID, "evolveClouds");

	uLightDir = glGetUniformLocation(programID, "lightDir");
	uLightColor = glGetUniformLocation(programID, "realLightColor");
	uLightFactor = glGetUniformLocation(programID, "lightFactor");
	uCloudColor = glGetUniformLocation(programID, "cloudColor");
	uZenitColor = glGetUniformLocation(programID, "zenitColor");
	uHorizonColor = glGetUniformLocation(programID, "horizonColor");
}

void VolumetricClouds::SetUniforms() 
{
	float earthRadius = 31000;
	float atmosphereStartOffset = 13000;
	float cloudRange = 4500;
	float innerSphereRadius = earthRadius + atmosphereStartOffset;
	float outerSphereRadius = innerSphereRadius + cloudRange;
	float sphereYOffset = -earthRadius;
	float cloudMaxRenderDistance = earthRadius + 4500;// innerSphereRadius * 5;


	float cloudTopOffset = 0.0f;
	float weatherTextureScale = 1.0f;
	float baseNoiseScale = 1.0f;
	float highFrequencyNoiseScale = 1.0f;
	float highFrequencyNoiseUVScale = 150.0f;
	float highFrequencyNoiseHScale = 4.0f;
	glm::vec3 cloudColor = glm::vec3(1, 1, 1);

	glm::vec3 zenitColor = glm::vec3(0.35f, 1.0f, 1.0f);
	glm::vec3 horizonColor = glm::vec3(0.7f, 0.3f, 1.0f);

	glm::vec3 lightColor = glm::vec3(1, 1, 1);
	float lightFactor = 0.95f;
	glm::vec3 realLightColor = glm::vec3(1, 1, 1);
	glm::vec3 lightDirection = glm::vec3(1, 1, 0.2);

	lightFactor = glm::clamp(glm::dot(glm::vec3(0, 1, 0), glm::normalize(lightDirection)), 0.0f, 1.0f);
	realLightColor = lightColor;
	realLightColor.y *= lightFactor * 0.85f;
	realLightColor.z *= lightFactor * 0.55f;
	glm::vec4 spherePos = glm::vec4(0, sphereYOffset,0, 1.0f);

	glm::mat4 invView = glm::inverse(Camera::cam->GetViewMatrix());
	glUniformMatrix4fv(uInvView, 1, GL_FALSE, &(invView[0][0]));

	glm::mat4 pV = Camera::cam->GetProjectionMatrix() * Camera::cam->GetViewMatrix();
	glUniformMatrix4fv(uProjView, 1, GL_FALSE, &(pV[0][0]));

	float time = Runner::TotalTimeInSeconds;
	glUniform1f(uTime, time);
	float incr = 0.015f;
	float rotation = time * incr;
	if (rotation > 360.0) {
		rotation -= 360;
	}
	glm::mat4 rot = glm::rotate(glm::mat4(1.0), rotation, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(uRotation, 1, GL_FALSE, &(rot[0][0]));


	glUniform3fv(uZenitColor, 1,&zenitColor[0]);
	glUniform3fv(uHorizonColor, 1, &horizonColor[0]);

	glUniform3fv(uLightDir, 1,&glm::normalize(lightDirection)[0]);
	glUniform3fv(uLightColor, 1, &realLightColor[0]);
	glUniform1f(uLightFactor, lightFactor);
	glUniform3fv(uCloudColor, 1, &cloudColor[0]);

	glUniform3fv(uSphereCenter, 1, &spherePos[0]);
	glUniform1f(uInnerSphereRadius, innerSphereRadius);
	glUniform1f(uOuterSphereRadius, outerSphereRadius);
	glUniform1f(uMaxDrawDistance, cloudMaxRenderDistance);
	glUniform1f(uCoverageMultiplier, coverageController);

	// set to 0.0 to disable evolution, 1.0 to enable.
	glUniform1f(uEvolveClouds, evolveClouds);

}
Mesh* VolumetricClouds::createQuad() {

	// Fullscreen quad in normalized device coordinates.
	// vert shader should be pass through, no transformations at all.
	float z = -1.0f;
	float coords = 1;
	Vertex v1(glm::vec3(-coords, -coords, z),glm::vec3(0,1,0), glm::vec2(0, 0));
	Vertex v2(glm::vec3(-coords, coords, z), glm::vec3(0, 1, 0), glm::vec2(1, 0));
	Vertex v3(glm::vec3(coords, -coords, z), glm::vec3(0, 1, 0), glm::vec2(0,1));
	Vertex v4(glm::vec3(coords, coords, z), glm::vec3(0, 1, 0), glm::vec2(1,1));
	vector<Vertex> vertices{ v1, v2, v3, v4 };
	vector<unsigned int> indices{ 0, 1, 2,
								 1, 3, 2 };
	vector<Texture> textures;
	return new Mesh(vertices, indices, textures);
}


unsigned int VolumetricClouds::generateTexture3D(int w, int h, int d) {
	unsigned int tex_output;
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, tex_output);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glTexStorage3D(GL_TEXTURE_3D, 6, GL_RGBA8, w, h, d);

	return tex_output;
}