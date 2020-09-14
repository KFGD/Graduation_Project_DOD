#include "stdafx.h"
#include "Entity.h"


Entity::Entity(const _uniqueId id)
	: mId(id)
{
}

_bool Entity::Initialize()
{
	return true;
}

Entity * Entity::Create(const _uniqueId id)
{
	Entity*	pInstance = new Entity(id);

	if (false == pInstance->Initialize())
	{
		MSG_BOX("Entity Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void Entity::Free()
{
}
