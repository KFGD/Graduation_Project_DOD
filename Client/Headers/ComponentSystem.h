#pragma once

#include "Base.h"

#include "Defines.h"

class ComponentSystem
	: public Base
{
protected:
	explicit ComponentSystem() = default;
	virtual ~ComponentSystem() = default;

public:
	virtual void	Update(const _double timeDelta) = 0;
	virtual void	LateUpdate(const _double timeDelta) = 0;

public:
	virtual void	Free()	override;
};