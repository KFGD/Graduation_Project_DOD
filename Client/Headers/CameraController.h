#pragma once
#include "Base.h"

#include "Defines.h"
#include "Client_Defines.h"

class Camera;

class CameraController :
	public Base
{
	DECLARE_SINGLETON(CameraController)

private:
	explicit CameraController();
	virtual ~CameraController() = default;

public:
	_bool	Ready();
	void	ChangeCamera(const CameraType::Type type);
	CameraType::Type	GetCurCameraType() const { return mCurType; }
	Camera*	GetCamera(const CameraType::Type type);

public:
	void	Update(const _double deltaTime);
	void	LateUpdate(const _double deltaTime);
	
private:
	array<Camera*, CameraType::TYPE_END>	mCameraList;
	CameraType::Type						mCurType = CameraType::TYPE_END;

public:
	virtual void	Free()	override;


};

