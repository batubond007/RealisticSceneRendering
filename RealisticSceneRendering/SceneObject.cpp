#include "SceneObject.h"
#include <iostream>

std::vector<SceneObject*> SceneObject::sceneObjects;

void SceneObject::Start()
{
	std::cout << "Started" << std::endl;
}

void SceneObject::Update()
{
	//std::cout << "Updated" << std::endl;
}

SceneObject::SceneObject()
{
	sceneObjects.push_back(this);
}
