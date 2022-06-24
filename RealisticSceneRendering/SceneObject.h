#pragma once
#include <vector>


class SceneObject
{
public:
	static std::vector<SceneObject*> sceneObjects;

	virtual void Start();
	virtual void Update();

	SceneObject();
};

