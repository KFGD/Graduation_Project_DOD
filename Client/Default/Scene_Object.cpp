#include "stdafx.h"
#include "Scene_Object.h"

#include "GameObject.h"

Scene_Object::Scene_Object()
{
}

void Scene_Object::Start()
{
}

void Scene_Object::Update(const _double timeDelta)
{
	for (GameObject* gameObject : mObjectList)
		gameObject->Update(timeDelta);

	for (GameObject* gameObject : mObjectList)
		gameObject->LateUpdate(timeDelta);
}

void Scene_Object::Render()
{
}

Scene::SCENE_TYPE Scene_Object::GetType() const
{
	return SCENE_TYPE::Object_Oriented;
}
