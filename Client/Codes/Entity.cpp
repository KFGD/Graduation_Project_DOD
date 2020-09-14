#include "stdafx.h"
#include "Entity.h"


Entity::Entity(const _uniqueId id)
	: mId(id)
{
}

Entity * Entity::Create(const _uniqueId id)
{
	return nullptr;
}

void Entity::Free()
{
}
