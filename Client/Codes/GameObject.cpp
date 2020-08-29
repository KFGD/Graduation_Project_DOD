#include "stdafx.h"
#include "GameObject.h"

#include "World_Object.h"

#include "ComponentManager_Object.h"
#include "Component_Object.h"

GameObject::GameObject()
{
}

void GameObject::SetUp(World_Object * world)
{
	mWorld = world;
	SafeAddRef(mWorld);
}

_bool GameObject::AddComponent(const char * prototypeTag, const char * componentTag, Component_Object ** component, void * arg)
{
	ComponentManager_Object* componentMgr = ComponentManager_Object::GetInstance();
	
	Component_Object*	tempComponent = nullptr;

	if (false == componentMgr->CloneComponent(prototypeTag, tempComponent, arg))
		return false;

	(*component) = tempComponent;

	mComponentMap.emplace(componentTag, (*component));
		
	return true;
}

_bool GameObject::DeleteComponent(const char * componentTag)
{
	COMPONENT_MAP::iterator iter = mComponentMap.find(componentTag);
	if (mComponentMap.end() == iter)
		return true;

	SafeRelease(iter->second);
	mComponentMap.erase(iter);

	return true;
}

void GameObject::Free()
{
	SafeRelease(mWorld);

	for (COMPONENT_PAIR& pair : mComponentMap)
		SafeRelease(pair.second);

	mComponentMap.clear();
}
