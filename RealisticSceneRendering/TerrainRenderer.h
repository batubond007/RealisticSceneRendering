#pragma once
#pragma once
#include "SceneObject.h"
#include "Camera.h"
#include "ObjImport.h"

class TerrainRenderer : public SceneObject
{
public:
	void Start() {
		SceneObject::Start();
		terrain = ObjImport::ParseObj("Mountain.obj");
		terrainShader = new Shader("terrain_vert.glsl", "terrain_frag.glsl");

		Texture tex("texture_diffuse", "Mountain_Diffuse.png");
		terrain->textures.push_back(tex);

		glm::mat4 modelingMatrix = glm::mat4(1);
		modelingMatrix = glm::translate(modelingMatrix, glm::vec3(0, -3, 0));

		terrainShader->use();
		terrainShader->setMat4("projectionMatrix", Camera::cam->GetProjectionMatrix());
		terrainShader->setMat4("viewingMatrix", Camera::cam->GetViewMatrix());
		terrainShader->setMat4("modelingMatrix", modelingMatrix);

	}

	void Update() {
		//SceneObject::Update();
		terrainShader->use();
		terrainShader->setMat4("viewingMatrix", Camera::cam->GetViewMatrix());
		terrain->Draw(*terrainShader);
	}

private:
	Mesh* terrain;
	Shader* terrainShader;
};