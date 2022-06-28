#pragma once
#include "SceneObject.h"
#include "Mesh.h"
#include "Camera.h"

class WaterRenderer : public SceneObject
{
public:
	void Start() {
		SceneObject::Start();
		quad = createQuad();
		waterShader = new Shader("water_vert.glsl", "water_frag.glsl");
	}

	void Update() {
		//SceneObject::Update();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);

		glm::mat4 modelingMatrix = glm::mat4(1);
		modelingMatrix = glm::translate(modelingMatrix, glm::vec3(-25, 0, -25));
		modelingMatrix = glm::scale(modelingMatrix, glm::vec3(50, 50, 50));

		waterShader->use();
		waterShader->setMat4("projectionMatrix", Camera::cam->GetProjectionMatrix());
		waterShader->setMat4("viewingMatrix", Camera::cam->GetViewMatrix());
		waterShader->setMat4("modelingMatrix", modelingMatrix);
		
		quad->Draw(*waterShader);
	}

private:
	Mesh* quad;
	Shader* waterShader;

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
};

