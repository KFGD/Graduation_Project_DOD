#include "stdafx.h"
#include "EntityManager.h"

#include "Entity.h"

IMPLEMENT_SINGLETON(EntityManager)

EntityManager::EntityManager()
{
}

Entity * EntityManager::CreateEntity()
{
	if (0 == mEntityIdQueue.size())
		return nullptr;

	const _uniqueId id = mEntityIdQueue.front();

	Entity* entity = Entity::Create(id);
	if (nullptr == entity)
		return nullptr;

	mEntityIdQueue.pop();
	return entity;
}

_bool EntityManager::Ready(const _size_t entitySize)
{
	if (0 != mEntitySize)
		return false;

	mEntitySize = entitySize;

	for (_uniqueId i = 0; i < mEntitySize; ++i)
		mEntityIdQueue.emplace(i);

	mEntityList.reserve(mEntitySize);

	return true;
}

void EntityManager::Clear()
{
	for (_size_t i = 0; i < mEntitySize; ++i)
		SafeRelease(mEntityList[i]);
	mEntityList.clear();
	mEntityList.shrink_to_fit();

	mEntityIdQueue.swap(queue<_uniqueId>());
	
	mEntitySize = 0;

}

void EntityManager::Free()
{
	Clear();
}
