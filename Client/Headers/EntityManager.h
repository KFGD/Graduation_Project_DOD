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
	Entity*	CreateEntity();

public:
	_bool	ReadySystem(const _size_t entitySize);
	void	ClearSystem();
	
private:
	_size_t				mEntitySize = 0;
	vector<Entity*>		mEntityList;
	queue<_uniqueId>	mEntityIdQueue;

public:
	virtual void	Free()	override;
};

