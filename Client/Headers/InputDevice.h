#pragma once
#include "Base.h"

#include "Defines.h"

class InputDevice :
	public Base
{
	DECLARE_SINGLETON(InputDevice)
public:
	enum MOUSEKEYSTATE { DIM_LBUTTON, DIM_RBUTTON, DIM_WHEELBUTTON, DIM_XBUTTON, DIM_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };

private:
	explicit InputDevice();
	virtual ~InputDevice() = default;

public:
	_byte	GetDIKeyState(_ubyte keyID) { return mKeyState[keyID]; }
	_byte	GetDIMouseState(MOUSEKEYSTATE mouse) { return mMouseState.rgbButtons[mouse]; }
	_long	GetDIMouseMove(MOUSEMOVESTATE mouseState) {
		return *(((_long*)&mMouseState) + mouseState);
	}
public:
	_bool Ready(HINSTANCE hInst, HWND hWnd);
	void	SetInputDev(void);

	_long	MouseMove(MOUSEMOVESTATE state);		// 마우스 움직임 변화량
	_byte	MouseState(MOUSEKEYSTATE mouse) { return mMouseState.rgbButtons[mouse]; }	// 마우스 클릭체크

private:
	LPDIRECTINPUT8			mInputSDK;
private:
	LPDIRECTINPUTDEVICE8	mKeyBoard;
	LPDIRECTINPUTDEVICE8	mMouse;
	_byte					mKeyState[256];
	DIMOUSESTATE			mMouseState;

public:
	virtual void	Free(void);

};

