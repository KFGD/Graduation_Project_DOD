#pragma once
#include "Base.h"

#include "Defines.h"

class AnimationCtrl final :
	public Base 
{
private:
	explicit AnimationCtrl(LPD3DXANIMATIONCONTROLLER dxAnimationCtrl);
	explicit AnimationCtrl(const AnimationCtrl& rhs);
	virtual ~AnimationCtrl() = default;

public:
	_bool	SetUpAnimation(const _uint index);
	_bool	PlayAnimation(const _double timeDelta);

private:
	LPD3DXANIMATIONCONTROLLER	mDxAnimationCtrl = nullptr;

private:
	_uint	mOldAniIndex = UINT_MAX;
	_uint	mCurrentTrack = 0;
	_uint	mNewTrack = 1;
	_double	mTimeDeltaAcc = 0.0;
	_double	mPeriod = 0.0;

public:
	static AnimationCtrl*	Create(LPD3DXANIMATIONCONTROLLER dxAnimationCtrl);
	AnimationCtrl*	Clone();
	virtual void	Free()	override;
};