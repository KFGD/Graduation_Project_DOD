#pragma once
#include "Base.h"

#include "Defines.h"

class AnimationCtrl_Object final :
	public Base
{
private:
	explicit AnimationCtrl_Object(LPD3DXANIMATIONCONTROLLER dxAnimationCtrl);
	explicit AnimationCtrl_Object(const AnimationCtrl_Object& rhs);
	virtual ~AnimationCtrl_Object() = default;

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
	static AnimationCtrl_Object*	Create(LPD3DXANIMATIONCONTROLLER dxAnimationCtrl);
	AnimationCtrl_Object*	Clone();
	virtual void	Free() override;

};

