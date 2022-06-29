#include "VolumetricClouds.h"
void VolumetricClouds::Start() {
	SceneObject::Start();
	quad = createQuad();
	cloudShader = new Shader("cloud_vert.glsl", "cloud_frag.glsl",nullptr);

	CacheUniformLocations();
	generateNoiseTextures();
}
void VolumetricClouds::generateNoiseTextures()
{
	Shader* perlinworleyComp = new Shader("perlinWorleyGen.comp");
	this->perlinTex = generateTexture3D(128, 128, 128);

	//glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	//glTexStorage3D(GL_TEXTURE_3D, 6, GL_RGBA8, 128, 128, 128);

	perlinworleyComp->use();
	std::cout << "computing perlinworley!" << std::endl;
	//glBindImageTexture(0, this->perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	glBindImageTexture(0, this->perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	perlinworleyComp->setInt("outNoise", 0);

	glDispatchCompute(128, 128, 128);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);


	glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	std::cout << "computed!!" << std::endl;

	this->perlinWorleyComputeID = perlinworleyComp->ID;

}


void VolumetricClouds::Update() {
	SceneObject::Update();

	glm::mat4 modelingMatrix = glm::mat4(1);
	//modelingMatrix = glm::translate(modelingMatrix, glm::vec3(-1, -1, -5));

	cloudShader->use();
	cloudShader->setMat4("projectionMatrix", Camera::cam->GetProjectionMatrix());
	cloudShader->setMat4("viewingMatrix", Camera::cam->GetViewMatrix());
	cloudShader->setMat4("modelingMatrix", modelingMatrix);
	cloudShader->setMat4("MVP", Camera::cam->GetProjectionMatrix() * Camera::cam->GetViewMatrix());
	cloudShader->setVec3("camPos", Camera::cam->Position);
	

	SetUniforms();

	cloudShader->setInt("perlinworley",0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, this->perlinTex);

	//glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	//cout << "CLOUDS draw" << endl;
	quad->Draw(*cloudShader);
	glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);

}

void VolumetricClouds::CacheUniformLocations()
{
	GLuint programID = cloudShader->ID;
	uSphereCenter = glGetUniformLocation(programID, "sphereCenter");
	uInnerSphereRadius = glGetUniformLocation(programID, "innerSphereRadius");
	uOuterSphereRadius = glGetUniformLocation(programID, "outerSphereRadius");
	uLightDir = glGetUniformLocation(programID, "lightDir");
	uCloudType = glGetUniformLocation(programID, "cloudType");
	uCoverageMultiplier = glGetUniformLocation(programID, "coverageMultiplier");

	uTopOffset = glGetUniformLocation(programID, "cloudTopOffset");
	uWeatherScale = glGetUniformLocation(programID, "weatherScale");
	uBaseNoiseScale = glGetUniformLocation(programID, "baseNoiseScale");
	uHighFreqNoiseScale = glGetUniformLocation(programID, "highFreqNoiseScale");
	uHighFreqNoiseUVScale = glGetUniformLocation(programID, "highFreqNoiseUVScale");
	uHighFreqNoiseHScale = glGetUniformLocation(programID, "highFreqNoiseHScale");
	uMaxDrawDistance = glGetUniformLocation(programID, "maxRenderDist");

	uProjView = glGetUniformLocation(programID, "projView");
	uInvView = glGetUniformLocation(programID, "invView");
}

void VolumetricClouds::SetUniforms() 
{
	float cloudType = 0.5f;
	float coverageMultiplier = 1.0f;
	//float innerSphereRadius = 10000000.0f * 2.0;
	//float outerSphereRadius = 10900000.0f * 2.0;
	//float sphereYOffset =	 -9600000.0f * 2.0;
	//float cloudMaxRenderDistance = 6000000.0f;

	float earthRadius = 31000;
	float atmosphereStartOffset = 12000;
	float cloudRange = 3100;
	float innerSphereRadius = earthRadius + atmosphereStartOffset;
	float outerSphereRadius = innerSphereRadius + cloudRange;
	float sphereYOffset = -earthRadius;
	float cloudMaxRenderDistance = 30000.0f;


	float cloudTopOffset = 0.0f;
	float weatherTextureScale = 1.0f;
	float baseNoiseScale = 1.0f;
	float highFrequencyNoiseScale = 1.0f;
	float highFrequencyNoiseUVScale = 150.0f;
	float highFrequencyNoiseHScale = 4.0f;
	glm::vec3 cloudColor = glm::vec3(1, 1, 1);


	//auto pos = Camera::cam->Position;
	//cout << "Cam pos: (" <<pos[0] << "," << pos[1] << "," << pos[2] << ")" << endl;
	glm::vec3 camPos = Camera::cam->Position;
	glm::vec4 spherePos = glm::vec4(camPos[0], sphereYOffset, camPos[2], 1.0f);

	glm::mat4 invView = glm::inverse(Camera::cam->GetViewMatrix());
	glUniformMatrix4fv(uInvView, 1, GL_FALSE, &(invView[0][0]));

	glm::mat4 pV = Camera::cam->GetProjectionMatrix() * Camera::cam->GetViewMatrix();
	glUniformMatrix4fv(uProjView, 1, GL_FALSE, &(pV[0][0]));


	glUniform3fv(uSphereCenter, 1, &spherePos[0]);
	glUniform1f(uInnerSphereRadius, innerSphereRadius);
	glUniform1f(uOuterSphereRadius, outerSphereRadius);
	glUniform1f(uMaxDrawDistance, cloudMaxRenderDistance);
	glUniform1f(uTopOffset, cloudTopOffset);
	glUniform1f(uWeatherScale, weatherTextureScale);
	glUniform1f(uBaseNoiseScale, baseNoiseScale);
	glUniform1f(uHighFreqNoiseScale, highFrequencyNoiseScale);
	glUniform1f(uHighFreqNoiseUVScale, highFrequencyNoiseUVScale);
	glUniform1f(uHighFreqNoiseHScale, highFrequencyNoiseHScale);

}
Mesh* VolumetricClouds::createQuad() {

	float z = -1.0f;
	Vertex v1(glm::vec3(-1000, -1000, z),glm::vec3(0,1,0), glm::vec2(0, 0));
	Vertex v2(glm::vec3(-1000, 1000, z), glm::vec3(0, 1, 0), glm::vec2(1, 0));
	Vertex v3(glm::vec3(1000, -1000, z), glm::vec3(0, 1, 0), glm::vec2(0,1));
	Vertex v4(glm::vec3(1000, 1000, z), glm::vec3(0, 1, 0), glm::vec2(1,1));
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

	glTexStorage3D(GL_TEXTURE_3D, 6, GL_RGBA8, 128, 128, 128);

	return tex_output;
}