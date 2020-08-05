#include "stdafx.h"
#include "ComponentManager_Object.h"

#include "Component_Object.h"

IMPLEMENT_SINGLETON(ComponentManager_Object)

ComponentManager_Object::ComponentManager_Object()
{
}

Component_Object * ComponentManager_Object::FindPrototype(const char * prototypeTag)
{
	PROTOTYPES::const_iterator iter = mPrototypes.find(prototypeTag);
	
	if (mPrototypes.end() == iter)
		return nullptr;

	return iter->second;
}

_bool ComponentManager_Object::AddPrototype(const char * prototypeTag, Component_Object * component)
{
	if (nullptr == component)
		return false;

	if (nullptr != FindPrototype(prototypeTag))
		return false;

	mPrototypes.emplace(prototypeTag, component);

	return true;
}

_bool ComponentManager_Object::CloneComponent(const char * prototypeTag, Component_Object *& component, void* arg)
{
	Component_Object* prototype = FindPrototype(prototypeTag);
	
	if (nullptr == prototype)
		return false;

	component = prototype->Clone(arg);

	if (nullptr == component)
		return false;

	return true;
}

_bool ComponentManager_Object::Clear()
{
	for (PAIR& pair : mPrototypes)
		SafeRelease(pair.second);
	mPrototypes.clear();

	return true;
}

void ComponentManager_Object::Free()
{
	Clear();
}
