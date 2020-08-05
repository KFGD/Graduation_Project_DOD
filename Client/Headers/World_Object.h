#pragma once
#include "World.h"

class PipeLine;

class ComponentManager_Object;
class GameObject;

class World_Object :
	public World
{
private:
	explicit World_Object(const LPDIRECT3DDEVICE9 graphicDev);
	explicit World_Object(const World& rhs) = delete;
	virtual ~World_Object() = default;

public:
	virtual void Update(const _double timeDelta) override;
	virtual void Render() override;
	virtual _bool Clear() override;

private:
	_bool	Initialize();
	_bool	ReadyComponent();
	_bool	ReadyGameObject();
	_bool	SetUpBot(const _vec3 position);

private:
	PipeLine*	mPipeLine = nullptr;
	ComponentManager_Object*	mComponentManager = nullptr;

private:
	vector<GameObject*>	mGameObjectList;

public:
	virtual void	Free() override;
	static World_Object*	Create(const LPDIRECT3DDEVICE9 graphicDev);
};

