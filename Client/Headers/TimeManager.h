#pragma once
#include "Base.h"

#include "Defines.h"

class Timer;

class TimeManager :
	public Base
{
	DECLARE_SINGLETON(TimeManager)

private:
	explicit TimeManager();
	virtual ~TimeManager() = default;

public:
	_double	GetTimeDelta(const char* const timerTag) { return GetTimeDelta(HashCode(timerTag)); }
	_double	GetTimeDelta(const _uint timeHash);

public:
	HRESULT	ReadyTimer(const char* const timerTag) { return ReadyTimer(HashCode(timerTag)); }
	HRESULT	ReadyTimer(const _uint timerHash);

private:
	map<const _uint, Timer*>	mTimerGroup;
	typedef	map<const _uint, Timer*>	TIMERGROUP;
	typedef pair<const _uint, Timer*>	TIMERPAIR;

private:
	Timer*	FindTimer(const _uint timerTag);

public:
	virtual	void	Free()	override;
};

