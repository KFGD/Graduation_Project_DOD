#pragma once
#include "Base.h"

#include "Defines.h"

class Frame :
	public Base
{
private:
	explicit Frame();
	virtual ~Frame() = default;

public:
	_bool	IsPermitCall(const _double timeDelta);

public:
	_bool	ReadyFrame(const _double callLimit);

private:
	_double	mCallLimit = 0.0;
	_double	mAccTimeDelta = 0.0;

public:
	static Frame*	Create(const _double callLimit);
	virtual void	Free() override;

};

