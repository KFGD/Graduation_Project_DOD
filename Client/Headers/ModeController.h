#pragma once
#include "Base.h"
#include "Defines.h"

#include "Client_Defines.h"

class IWorldController;
class CreativeMode;

class ModeController :
	public Base
{
	DECLARE_SINGLETON(ModeController)

private:
	explicit ModeController();
	virtual ~ModeController() = default;

public:
	_bool	Ready(LPDIRECT3DDEVICE9 graphicDevice);

public:
	void	Update(IWorldController* worldController);
	void	Render(LPDIRECT3DDEVICE9 graphicDevice);

private:
	void	UpdateModeController(IWorldController* worldController);

private:
	_bool	mIsWindowMode = true;

private:
	Program::Mode	mCurMode = Program::Mode::GAME;
	_bool	mbShowWindow = true;

private:
	CreativeMode*	mCreativeMode = nullptr;

public:
	virtual	void	Free() override;

};

