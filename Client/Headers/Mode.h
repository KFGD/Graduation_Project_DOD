#pragma once

#include "Base.h"
#include "Defines.h"

class IWorldController;

class Mode :
	public Base
{
protected:
	explicit Mode() = default;
	virtual ~Mode() = default;

public:
	virtual void	Active() = 0;
	virtual void	InActive() = 0;
	virtual void	Update(const _double timeDelta) = 0;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice) = 0;

public:
	virtual void	Free() override;
};