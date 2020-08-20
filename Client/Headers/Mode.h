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
	virtual void	Active(IWorldController* worldController) = 0;
	virtual void	InActive(IWorldController* worldController) = 0;
	virtual void	Update(IWorldController* worldController) = 0;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice) = 0;

public:
	virtual void	Free() override;
};