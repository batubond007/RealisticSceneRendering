#include "Runner.h"
#include "WaterRenderer.h"

void Runner::DrawScene() {
	ClearBits();
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
	{
		SceneObject::sceneObjects[i]->Update();
	}
}

void Runner::DrawSceneWOWater() {
	ClearBits();
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
	{
		if (dynamic_cast<const WaterRenderer*>(SceneObject::sceneObjects[i]) == nullptr)
		{
			SceneObject::sceneObjects[i]->Update();
		}
	}
}

void Runner::ClearBits()
{
	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
