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
	static const _vec3 AXIS_Y;

public:
	virtual void	Free()	override;
};

