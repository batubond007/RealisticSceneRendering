#include "Runner.h"
#include "WaterRenderer.h"

void Runner::DrawScene() {
	for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
	{
		SceneObject::sceneObjects[i]->Update();
	}
}

void Runner::DrawSceneWOWater() {
	for (int i = 0; i < SceneObject::sceneObjects.size(); i++)
	{
		if (dynamic_cast<const WaterRenderer*>(SceneObject::sceneObjects[i]) == nullptr)
		{
			SceneObject::sceneObjects[i]->Update();
		}
	}
}