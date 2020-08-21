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
	_bool	GetLocking() const { return mIsLocking; }
	void	SetLocking(const _bool value) { mIsLocking = value; }
	_float	GetMoveSpeed() const { return mMoveSpeed; }
	void	SetMoveSpeed(const _float moveSpeed) { mMoveSpeed = moveSpeed; }

private:
	_bool	Initialize();
	const _vec3&	GetRightVector()	const	{ return *((_vec3*)mWorldMatrix.m[0]); }
	const _vec3&	GetUpVector()		const	{ return *((_vec3*)mWorldMatrix.m[1]); }
	const _vec3&	GetForwardVector()	const	{ return *((_vec3*)mWorldMatrix.m[2]); }
	const _vec3&	GetPosition()		const	{ return *((_vec3*)mWorldMatrix.m[3]); }
	
	void	SetRightVector(const _vec3& rightVector) { (*(_vec3*)mWorldMatrix.m[0]) = rightVector; }
	void	SetUpVector(const _vec3& upVector) { (*(_vec3*)mWorldMatrix.m[1]) = upVector; }
	void	SetForwardVector(const _vec3& forwardVector) { (*(_vec3*)mWorldMatrix.m[2]) = forwardVector; }
	void	SetPosition(const _vec3& position) { *((_vec3*)mWorldMatrix.m[3]) = position; }

private:
	_bool	mIsLocking = false;
	_float	mMoveSpeed = 0.f;
private:
	_matrix	mWorldMatrix;

public:
	static	FreeCamera*	Create();
	virtual	void	Free()	override;
};

