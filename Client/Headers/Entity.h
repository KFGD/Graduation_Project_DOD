#pragma once
#include "Base.h"

#include "Defines.h"

class Entity :
	public Base
{
private:
	explicit Entity(const _uniqueId id);
	virtual ~Entity() = default;

public:
	_uniqueId	GetUniqueId() const { return mId; }

private:
	_bool		Initialize();

private:
	_uniqueId mId;

public:
	static Entity*	Create(const _uniqueId id);
	virtual void Free() override;
};

