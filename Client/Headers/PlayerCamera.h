#pragma once
#include "Camera.h"

class Transform_Object;
class CameraTarget;

class PlayerCamera :
	public Camera
{
private:
	explicit PlayerCamera();
	virtual ~PlayerCamera() = default;

public:
	virtual void Active(CameraController * cameraController)	override;
	virtual void InActive(CameraController * cameraController)	override;
	virtual void Update(const _double deltaTime)				override;
	virtual void LateUpdate(const _double deltaTime)			override;

public:
	void		SetTarget(CameraTarget* mTarget) { mTarget = mTarget; }

private:
	_bool		Initialize();

private:
	CameraTarget*	mTarget = nullptr;
	_float			mRadian = 0.f;
	_float			mDist = 0.f;
	_vec3			mLook;

public:
	static	PlayerCamera*	Create();
	virtual	void			Free()		override;
};

