#pragma once
#include "Base.h"

#include "Defines.h"
#include "Client_Defines.h"

class Camera;
class CameraTarget;

class CameraController :
	public Base
{
	DECLARE_SINGLETON(CameraController)

private:
	explicit CameraController();
	virtual ~CameraController() = default;

public:
	_bool				Ready();
	void				ChangeCamera(const CameraType::Type type);
	CameraType::Type	GetCurCameraType() const { return mCurType; }
	Camera*				GetCamera(const CameraType::Type type);
	void				SetLocking(const _bool isLocking) { mIsLocking = isLocking; }
	_bool				GetLocking() const { return mIsLocking; }
	CameraTarget*		GetCameraTarget() { return mCameraTarget; }
	void				SetCameraTarget(CameraTarget* cameraTarget) { mCameraTarget = cameraTarget; }
	

public:
	void	Update(const _double deltaTime);
	void	LateUpdate(const _double deltaTime);
	
private:
	array<Camera*, CameraType::TYPE_END>	mCameraList;
	CameraType::Type						mCurType = CameraType::TYPE_END;

private:
	_bool			mIsLocking = true;
	CameraTarget*	mCameraTarget = nullptr;

public:
	virtual void	Free()	override;


};

