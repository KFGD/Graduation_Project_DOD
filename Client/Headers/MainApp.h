#pragma once
#include "Base.h"

#include "Defines.h"

//	System
class GraphicDevice;

//	Core
class World;

class ModeController;

class MainApp final : public Base
{
private:
	enum MODE
	{
		OOD, DOD
	};
private:
	explicit MainApp();
	virtual ~MainApp() = default;

public:
	bool Initialize();

public:
	int		Update(const double tiemDelta);
	bool	Render();
	
private:
	bool	ReadySystem(const HWND hWnd, const bool bWinMode, const UINT sizeX, const UINT sizeY);
	bool	ReadyWorld();
	
private:
	GraphicDevice* mGraphicDeviceSys = nullptr;
	
private:
	ModeController*		mModeController = nullptr;

private:
	LPDIRECT3DDEVICE9	mGraphicDevice = nullptr;

private:
	//	World
	MODE	mCurMode;
	World*	mCurWorld = nullptr;
	array<World*, 2>	mWorlds;

public:
	static MainApp*	Create();
	virtual	void	Free()	override;

};
