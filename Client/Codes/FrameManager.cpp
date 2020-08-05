#include "stdafx.h"
#include "FrameManager.h"

#include "Frame.h"

IMPLEMENT_SINGLETON(FrameManager)

FrameManager::FrameManager()
{
}

_bool FrameManager::IsPermitCall(const _uint frameHash, const _double timeDelta)
{
	Frame*	pInstance = FindFrame(frameHash);
	if (nullptr == pInstance)
		return false;

	return pInstance->IsPermitCall(timeDelta);
}

_bool FrameManager::ReadyFrame(const _uint frameHash, const _double callLimit)
{
	Frame*	pInstance = FindFrame(frameHash);

	if (nullptr != pInstance)
		return false;

	pInstance = Frame::Create(callLimit);
	if (nullptr == pInstance)
		return false;

	mFrameGroup.emplace(frameHash, pInstance);

	return true;
}

Frame * FrameManager::FindFrame(const _uint frameHash)
{
	FRAMEGROUP::const_iterator&	iter = mFrameGroup.find(frameHash);

	if (mFrameGroup.end() == iter)
		return nullptr;

	return iter->second;
}

void FrameManager::Free()
{
	for (FRAMEPAIR& pair : mFrameGroup)
		SafeRelease(pair.second);

	mFrameGroup.clear();
}
