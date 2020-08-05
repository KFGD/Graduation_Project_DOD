#pragma once
#include "Base.h"

#include "Defines.h"

class Component_Object :
	public Base
{
protected:
	explicit Component_Object() = default;
	explicit Component_Object(const Component_Object& rhs) = default;
	virtual ~Component_Object() = default;

public:
	virtual void	Free() override;
	virtual Component_Object*	Clone(void* arg) = 0;

};

