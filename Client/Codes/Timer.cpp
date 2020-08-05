#include "stdafx.h"
#include "Timer.h"


Timer::Timer()
{
	::ZeroMemory(&mFrameTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&mLastTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&mFixTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&mCpuTick, sizeof(LARGE_INTEGER));
}

HRESULT Timer::Ready()
{
	QueryPerformanceCounter(&mFrameTime);
	QueryPerformanceCounter(&mLastTime);
	QueryPerformanceCounter(&mFixTime);
	QueryPerformanceFrequency(&mCpuTick);

	return NOERROR;
}

double Timer::GetTimeDelta()
{
	QueryPerformanceCounter(&mFrameTime);

	if (mFrameTime.QuadPart - mFixTime.QuadPart >= mCpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&mCpuTick);
		mFixTime = mFrameTime;
	}

	mTimeDelta = ((_double)mFrameTime.QuadPart - (_double)mLastTime.QuadPart) / (_double)mCpuTick.QuadPart;
	
	mLastTime = mFrameTime;

	return mTimeDelta;
}

Timer * Timer::Create()
{
	Timer*	pInstance = new Timer;

	if (FAILED(pInstance->Ready()))
	{
		MSG_BOX("Timer Create Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void Timer::Free()
{
}

