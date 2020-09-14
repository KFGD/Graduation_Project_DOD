#include "stdafx.h"
#include "EntityManager.h"

#include "Entity.h"

EntityManager::EntityManager()
{
}

_bool EntityManager::Ready(const _size_t entitySize)
{
	if (0 != mEntitySize)
		return false;

	mEntitySize = entitySize;

	for (_uniqueId i = 0; i < mEntitySize; ++i)
		mIdQueue.emplace(i);

	mEntityList.reserve(mEntitySize);

	return true;
}

void EntityManager::Clear()
{
	for (_size_t i = 0; i < mEntitySize; ++i)
		SafeRelease(mEntityList[i]);
	mEntityList.clear();

	mIdQueue.swap(queue<_uniqueId>());
	
	mEntitySize = 0;

}

Entity * EntityManager::CreateEntity()
{
	if (0 == mIdQueue.size())
		return nullptr;

	const _uniqueId id = mIdQueue.front();
	
	Entity* entity = Entity::Create(id);
	if (nullptr == entity)
		return nullptr;

	mIdQueue.pop();
	return entity;
}

void EntityManager::Free()
{
	Clear();
}
