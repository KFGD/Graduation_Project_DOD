#include "stdafx.h"
#include "Frame.h"


Frame::Frame()
{
}

_bool Frame::IsPermitCall(const _double timeDelta)
{
	mAccTimeDelta += timeDelta;

	if (mAccTimeDelta >= mCallLimit)
	{
		mAccTimeDelta = 0.0;
		return true;
	}

	return false;
}

_bool Frame::ReadyFrame(const _double callLimit)
{
	mCallLimit = 1.0 / callLimit;
	return true;
}

Frame * Frame::Create(const _double callLimit)
{
	Frame*	pInstance = new Frame;

	if (FAILED(pInstance->ReadyFrame(callLimit)))
		SafeRelease(pInstance);

	return pInstance;
}

void Frame::Free()
{
}

