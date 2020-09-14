#pragma once
#include "Base.h"

#include "Defines.h"

class Entity;

class EntityManager :
	public Base
{
	DECLARE_SINGLETON(EntityManager)
private:
	explicit EntityManager();
	virtual ~EntityManager() = default;
	
public:
	_bool	Ready(const _size_t entitySize);
	void	Clear();

	Entity*	CreateEntity();
	
private:
	_size_t				mEntitySize;
	vector<Entity*>		mEntityList;
	queue<_uniqueId>	mIdQueue;

public:
	virtual void	Free()	override;
};

