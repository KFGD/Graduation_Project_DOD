#pragma once
#include "Base.h"
#include "WorldController.h"

//	System
class GraphicDevice;
class InputDevice;
class World;

class ModeController;
class CameraController;
class KeyManager;

class MainApp final : 
	public Base, public IWorldController
{
private:
	explicit MainApp();
	virtual ~MainApp() = default;

private:
	bool Initialize();

public:
	virtual _bool	ChangeWorld(const Game::WorldType worldType)		override;
	virtual _bool	SetUpObjectList(const vector<KObject*>& objectList)	override;
	virtual _bool	ClearObjectList()									override;

public:
	int		Update(const double tiemDelta);
	bool	Render();
	
private:
	bool	ReadySystem(const HWND hWnd, const bool bWinMode, const UINT sizeX, const UINT sizeY);
	bool	ReadyWorld();
	
private:
	GraphicDevice*	mGraphicDeviceSys = nullptr;
	InputDevice*	mInputDevice = nullptr;

private:
	ModeController*		mModeController = nullptr;
	CameraController*	mCameraController = nullptr;
	KeyManager*			mKeyManager = nullptr;

private:
	LPDIRECT3DDEVICE9	mGraphicDevice = nullptr;

private:
	//	World
	Game::WorldType	mCurMode;
	array<World*, Game::WorldEnd>	mWorlds;
	
public:
	static MainApp*	Create();
	virtual	void	Free()	override;

};
