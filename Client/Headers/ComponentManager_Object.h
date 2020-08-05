#pragma once
#include "Base.h"

#include "Defines.h"

class Component_Object;

class ComponentManager_Object :
	public Base
{
	DECLARE_SINGLETON(ComponentManager_Object)

private:
	explicit ComponentManager_Object();
	virtual ~ComponentManager_Object() = default;

public:
	Component_Object*	FindPrototype(const char* prototypeTag);
	_bool	AddPrototype(const char* prototypeTag, Component_Object* component);
	_bool	CloneComponent(const char* prototypeTag, Component_Object*& component, void* arg);
	_bool	Clear();

private:
	unordered_map<string, Component_Object*>	mPrototypes;
	typedef	unordered_map<string, Component_Object*>	PROTOTYPES;
	typedef pair<const string, Component_Object*>	PAIR;

public:
	virtual void	Free() override;

};

