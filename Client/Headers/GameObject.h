#pragma once
#include "Base.h"

#include "Defines.h"

class Component_Object;
class World_Object;

class GameObject :
	public Base
{
protected:
	explicit GameObject();
	virtual ~GameObject() = default;


public:
	virtual void	SetUp(World_Object* world);
	virtual void	Update(const _double timeDelta) = 0;
	virtual void	LateUpdate(const _double timeDelta) = 0;
	virtual void	Render() = 0;

protected:
	World_Object*	GetWorld() { return mWorld; }
	_bool	AddComponent(const char* prototypeTag, const char* componentTag, Component_Object** component, void* arg = nullptr);
	_bool	DeleteComponent(const char* componentTag);

private:
	World_Object*	mWorld = nullptr;

private:
	unordered_map<string, Component_Object*>			mComponentMap;
	typedef	unordered_map<string, Component_Object*>	COMPONENT_MAP;
	typedef pair<const string, Component_Object*>		COMPONENT_PAIR;

public:
	virtual void Free() override;
};

