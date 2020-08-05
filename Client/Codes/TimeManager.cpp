#include "stdafx.h"
#include "TimeManager.h"

#include "Timer.h"

IMPLEMENT_SINGLETON(TimeManager)

TimeManager::TimeManager()
{
}

_double TimeManager::GetTimeDelta(const _uint timerHash)
{
	Timer*	pInstance = FindTimer(timerHash);
	if (nullptr == pInstance)
		return -1.0;

	return pInstance->GetTimeDelta();
}

HRESULT TimeManager::ReadyTimer(const _uint timerHash)
{
	Timer*	pInstance = FindTimer(timerHash);

	if (nullptr != pInstance)
		return E_FAIL;

	pInstance = Timer::Create();
	if (nullptr == pInstance)
		return E_FAIL;

	mTimerGroup.emplace(timerHash, pInstance);

	return NOERROR;
}

Timer * TimeManager::FindTimer(const _uint timerHash)
{
	TIMERGROUP::const_iterator& iter = mTimerGroup.find(timerHash);

	if (mTimerGroup.end() == iter)
		return nullptr;

	return iter->second;
}

void TimeManager::Free()
{
	for (TIMERPAIR& pair : mTimerGroup)
		SafeRelease(pair.second);

	mTimerGroup.clear();
}
