#pragma once
#include "Scene.h"

class GameObject;

class Scene_Object :
	public Scene
{
private:
	explicit Scene_Object();
	virtual ~Scene_Object() = default;

public:
	virtual void Start() override;
	virtual void Update(const _double timeDelta) override;
	virtual void Render() override;

public:
	virtual SCENE_TYPE GetType() const override;

private:
	list<GameObject*>	mObjectList;

};

