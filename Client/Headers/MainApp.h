#pragma once
#include "Base.h"

#include "Defines.h"
#include "Client_Defines.h"

//	System
class GraphicDevice;
class InputDevice;

class ModeController;
class CameraController;
class KeyManager;

class MainApp final : 
	public Base
{
private:
	explicit MainApp();
	virtual ~MainApp() = default;

private:
	bool Initialize();

public:
	int		Update(const double timeDelta);
	bool	Render();
	
private:
	bool	ReadySystem(const HWND hWnd, const bool bWinMode, const UINT sizeX, const UINT sizeY);
	
private:
	GraphicDevice*	mGraphicDeviceSys = nullptr;
	InputDevice*	mInputDevice = nullptr;

private:
	ModeController*		mModeController = nullptr;
	CameraController*	mCameraController = nullptr;
	KeyManager*			mKeyManager = nullptr;

private:
	LPDIRECT3DDEVICE9	mGraphicDevice = nullptr;

public:
	static MainApp*	Create();
	virtual	void	Free()	override;

};
