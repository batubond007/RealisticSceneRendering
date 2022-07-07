#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include "Camera.h"
#include "Window.h"
#include "Runner.h"
#include "TerrainRenderer.h"

class WaterRenderer : public SceneObject
{
public:
	static float WaveSpeed;

	void Start() {
		SceneObject::Start();
		quad = createQuad();
		waterShader = new Shader("water_vert.glsl", "water_frag.glsl");
		dudvMap = (new Texture("Map", "WaterDuDv.png", false))->id;
		normalMap = (new Texture("Map", "WaterNormalMap.png", false))->id;
	}

	void Update() {
		// Calculate Matrices
		glm::mat4 modelingMatrix = glm::mat4(1);
		modelingMatrix = glm::translate(modelingMatrix, glm::vec3(-5000, 0, -5000));
		modelingMatrix = glm::scale(modelingMatrix, glm::vec3(10000, 10000, 10000));

		//Add Time to MoveFactor
		MoveFactor += WaveSpeed * Runner::DeltaTime;
		MoveFactor = fmod(MoveFactor, 1.0f);
		
		// Create reflection Attachments
		GLuint reflectionFrameBuffer = createFrameBuffer();
		GLuint reflectionTexture = createTextureAttachment(1920, 1080);
		GLuint reflectionDepthBuffer = createDepthBufferAttachment(1920, 1080);
		unbindFrameBuffer();
		
		// Create refraction Attachments
		GLuint refractionFrameBuffer = createFrameBuffer();
		GLuint refractionTexture = createTextureAttachment(1920, 1080);
		GLuint refractionDepthTexture = createDepthTextureAttachment(1920, 1080);
		unbindFrameBuffer();

		glEnable(GL_CLIP_DISTANCE0);
		//Render refraction
		bindFrameBuffer(refractionFrameBuffer, 1920, 1080);
		TerrainRenderer::terrainShader->setVec4("clipPlane", glm::vec4(0, -1, 0, 0));
		Runner::DrawSceneWOWater();
		unbindFrameBuffer();

		//Render reflection
		Camera::cam->InvertCam();

		bindFrameBuffer(reflectionFrameBuffer, 1920, 1080);
		TerrainRenderer::terrainShader->setVec4("clipPlane", glm::vec4(0, 1, 0, 0));
		Runner::DrawSceneWOWater();
		unbindFrameBuffer();

		Camera::cam->RecoverCam();

		glDisable(GL_CLIP_DISTANCE0);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);

		waterShader->use();
		// Set Uniforms
		waterShader->setMat4("projectionMatrix", Camera::cam->GetProjectionMatrix());
		waterShader->setMat4("viewingMatrix", Camera::cam->GetViewMatrix());
		waterShader->setMat4("modelingMatrix", modelingMatrix);
		waterShader->setSampler2D("refractionTex", refractionTexture, 0);
		waterShader->setSampler2D("reflectionTex", reflectionTexture, 1);
		waterShader->setSampler2D("dudvMap", dudvMap, 2);
		waterShader->setSampler2D("normalMap", normalMap, 3);
		waterShader->setFloat("MoveFactor", MoveFactor);
		waterShader->setVec3("eyePos", Camera::cam->Position);
		quad->Draw(*waterShader);

		glDeleteFramebuffers(1, &reflectionFrameBuffer);
		glDeleteTextures(1, &reflectionTexture);
		glDeleteRenderbuffers(1, &reflectionDepthBuffer);

		glDeleteFramebuffers(1, &refractionFrameBuffer);
		glDeleteTextures(1, &refractionTexture);
		glDeleteTextures(1, &refractionDepthTexture);

	}

private:
	Mesh* quad;
	Shader* waterShader;
	GLuint dudvMap;
	GLuint normalMap;
	float MoveFactor = 0;

	Mesh* createQuad() {
		Vertex v1(glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
		Vertex v2(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		Vertex v3(glm::vec3(1, 0, 1), glm::vec3(0, 1, 0));
		Vertex v4(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
		vector<Vertex> vertices{ v1, v2, v3, v4 };
		vector<unsigned int> indices{0, 1, 2,
									 1, 3, 2};
		vector<Texture> textures;
		return new Mesh(vertices, indices, textures);
	}

	int createFrameBuffer() {
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		//generate name for frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		//create the framebuffer
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//indicate that we will always render to color attachment 0
		return frameBuffer;
	}

	int createTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			texture, 0);
		return texture;
	}

	int createDepthTextureAttachment(int width, int height) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			texture, 0);
		return texture;
	}

	int createDepthBufferAttachment(int width, int height) {
		GLuint depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width,
			height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthBuffer);
		return depthBuffer;
	}

	void bindFrameBuffer(int frameBuffer, int width, int height) {
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glViewport(0, 0, width, height);
	}

	void unbindFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Window::width, Window::height);
	}
};

