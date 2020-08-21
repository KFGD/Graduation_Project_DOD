#pragma once
#include "Base.h"
#include "Defines.h"

#include "Client_Defines.h"

class IWorldController;
class CameraController;
class Mode;

class ModeController :
	public Base
{
	DECLARE_SINGLETON(ModeController)

private:
	explicit ModeController();
	virtual ~ModeController() = default;

public:
	_bool	Ready(LPDIRECT3DDEVICE9 graphicDevice);
	Program::Mode	GetCurProgramMode() const { return mCurMode; }

public:
	void	Update(IWorldController* worldController, CameraController* cameraController);
	void	Render(LPDIRECT3DDEVICE9 graphicDevice);

private:
	void	UpdateModeControllerUI(IWorldController* worldController);
	void	UpdateCameraControllerUI(CameraController* cameraController);

private:
	void	MappingDataToCameraController(CameraController* cameraController);
	void	MappingCameraControllerToData(CameraController* cameraController);
	
private:
	_bool	mIsWindowMode = true;

private:
	Program::Mode	mCurMode = Program::Mode::GAME;
	_bool	mIsShowModeController = true;
	array<Mode*, Program::MODE_END>	mMode;

private:
	_bool				mIsShowCameraController = true;
	CameraType::Type	mCurCameraType = CameraType::FREE_CAMERA;

private:
	//	Free Camera
	_float	mFreeCameraMoveSpeed = 0.f;
	_bool	mIsFreeCameraLocking = false;
	
public:
	virtual	void	Free() override;

};

