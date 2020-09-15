#include "stdafx.h"
#include "AnimationCtrl.h"

AnimationCtrl::AnimationCtrl(LPD3DXANIMATIONCONTROLLER dxAnimationCtrl)
	: mDxAnimationCtrl(dxAnimationCtrl)
{
	SafeAddRef(mDxAnimationCtrl);
}

AnimationCtrl::AnimationCtrl(const AnimationCtrl & rhs)
	: mDxAnimationCtrl(nullptr)
{
}

_bool AnimationCtrl::SetUpAnimation(const _uint index)
{
	if (nullptr == mDxAnimationCtrl)
		return false;

	if (mOldAniIndex == index)
		return true;

	LPD3DXANIMATIONSET		animationSet = nullptr;

	if (FAILED(mDxAnimationCtrl->GetAnimationSet(index, &animationSet)))
		return false;

	mPeriod = animationSet->GetPeriod();

	mNewTrack = mCurrentTrack == 0 ? 1 : 0;

	mDxAnimationCtrl->SetTrackAnimationSet(mNewTrack, animationSet);

	mDxAnimationCtrl->UnkeyAllTrackEvents(mCurrentTrack);
	mDxAnimationCtrl->UnkeyAllTrackEvents(mNewTrack);

	mDxAnimationCtrl->KeyTrackEnable(mCurrentTrack, FALSE, mTimeDeltaAcc + 0.2);
	mDxAnimationCtrl->KeyTrackSpeed(mCurrentTrack, 1.f, mTimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);
	mDxAnimationCtrl->KeyTrackWeight(mCurrentTrack, 0.1f, mTimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);

	mDxAnimationCtrl->SetTrackEnable(mNewTrack, TRUE);
	mDxAnimationCtrl->KeyTrackSpeed(mNewTrack, 1.f, mTimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);
	mDxAnimationCtrl->KeyTrackWeight(mNewTrack, 0.9f, mTimeDeltaAcc, 0.2, D3DXTRANSITION_LINEAR);

	mDxAnimationCtrl->SetTrackPosition(mNewTrack, 0.0);

	mDxAnimationCtrl->ResetTime();

	mTimeDeltaAcc = 0.0;

	mOldAniIndex = index;

	mCurrentTrack = mNewTrack;


	return true;
}

_bool AnimationCtrl::PlayAnimation(const _double timeDelta)
{
	if (nullptr == mDxAnimationCtrl)
		return false;

	mDxAnimationCtrl->AdvanceTime(timeDelta, nullptr);

	mTimeDeltaAcc += timeDelta;

	return NOERROR;
}

AnimationCtrl * AnimationCtrl::Create(LPD3DXANIMATIONCONTROLLER dxAnimationCtrl)
{
	return new AnimationCtrl(dxAnimationCtrl);
}

AnimationCtrl * AnimationCtrl::Clone()
{
	AnimationCtrl* pInstance = new AnimationCtrl(*this);

	if (FAILED(mDxAnimationCtrl->CloneAnimationController(mDxAnimationCtrl->GetMaxNumAnimationOutputs(), mDxAnimationCtrl->GetMaxNumAnimationSets(), mDxAnimationCtrl->GetMaxNumTracks(), mDxAnimationCtrl->GetMaxNumEvents(), &pInstance->mDxAnimationCtrl)))
	{
		MSG_BOX("AnimationCtrl Clone Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void AnimationCtrl::Free()
{
	SafeRelease(mDxAnimationCtrl);
}
