#pragma once
#include "Base.h"

#include "Defines.h"

class Timer :
	public Base
{
private:
	explicit Timer();
	virtual ~Timer() = default;

public:
	HRESULT	Ready();
	_double	GetTimeDelta();

private:
	LARGE_INTEGER	mFrameTime;
	LARGE_INTEGER	mLastTime;
	LARGE_INTEGER	mFixTime;
	LARGE_INTEGER	mCpuTick;

	_double	mTimeDelta = 0.0;

public:
	static	Timer*	Create();
	virtual void	Free() override;

};

