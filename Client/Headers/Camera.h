#pragma once
#include "Base.h"

#include "Defines.h"

class CameraController;

class Camera :
	public Base
{
protected:
	explicit Camera();
	virtual ~Camera() = default;

public:
	virtual void	Active(CameraController* cameraController) = 0;
	virtual void	InActive(CameraController* cameraController) = 0;
	virtual	void	Update(const _double deltaTime) = 0;
	virtual void	LateUpdate(const _double deltaTime) = 0;

protected:
	const _vec3&	GetRightVector()	const { return *((_vec3*)mWorldMatrix.m[0]); }
	const _vec3&	GetUpVector()		const { return *((_vec3*)mWorldMatrix.m[1]); }
	const _vec3&	GetForwardVector()	const { return *((_vec3*)mWorldMatrix.m[2]); }
	const _vec3&	GetPosition()		const { return *((_vec3*)mWorldMatrix.m[3]); }
	const _matrix&	GetWorldMatrix()	const { return mWorldMatrix; }

	void	SetRightVector(const _vec3& rightVector) { (*(_vec3*)mWorldMatrix.m[0]) = rightVector; }
	void	SetUpVector(const _vec3& upVector) { (*(_vec3*)mWorldMatrix.m[1]) = upVector; }
	void	SetForwardVector(const _vec3& forwardVector) { (*(_vec3*)mWorldMatrix.m[2]) = forwardVector; }
	void	SetPosition(const _vec3& position) { *((_vec3*)mWorldMatrix.m[3]) = position; }
	void	SetWorldMatrix(const _matrix& worldMatrix) { mWorldMatrix = worldMatrix; }

protected:
	void	Rotate(const _matrix& rotationMatrix);

protected:
	static const _vec3 AXIS_Y;

private:
	_matrix	mWorldMatrix;

public:
	virtual void	Free()	override;
};

