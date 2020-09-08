#pragma once
#include "Camera.h"

class FreeCamera :
	public Camera
{
public:

private:
	explicit FreeCamera();
	virtual ~FreeCamera() = default;

public:
	virtual void Active(CameraController * cameraController) override;
	virtual void InActive(CameraController * cameraController) override;
	virtual void Update(const _double deltaTime) override;
	virtual void LateUpdate(const _double deltaTime) override;

public:
	void	ResetRotation();
	_float	GetMoveSpeed() const { return mMoveSpeed; }
	void	SetMoveSpeed(const _float moveSpeed) { mMoveSpeed = moveSpeed; }

private:
	_bool	Initialize();

private:
	_float	mMoveSpeed = 0.f;

public:
	static	FreeCamera*	Create();
	virtual	void	Free()	override;
};

