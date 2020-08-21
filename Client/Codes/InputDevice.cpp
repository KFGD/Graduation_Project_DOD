#include "stdafx.h"
#include "InputDevice.h"

IMPLEMENT_SINGLETON(InputDevice)

InputDevice::InputDevice(void)
	: mInputSDK(nullptr)
	, mKeyBoard(nullptr)
	, mMouse(nullptr)
{

}

_bool InputDevice::Ready(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED((DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&mInputSDK,
		NULL))))
		return false;

	if (FAILED(mInputSDK->CreateDevice(GUID_SysKeyboard, &mKeyBoard, nullptr)))
		return false;

	mKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	mKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	mKeyBoard->Acquire();

	if (FAILED(mInputSDK->CreateDevice(GUID_SysMouse, &mMouse, nullptr)))
		return E_FAIL;

	mMouse->SetDataFormat(&c_dfDIMouse);

	mMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	mMouse->Acquire();


	return true;
}

void InputDevice::SetInputDev(void)
{
	if (nullptr == mKeyBoard ||
		nullptr == mMouse)
		return;

	mKeyBoard->GetDeviceState(256, mKeyState);
	mMouse->GetDeviceState(sizeof(mMouseState), &mMouseState);
}

_long InputDevice::MouseMove(MOUSEMOVESTATE state)
{
	return *(((_long*)&mMouseState) + state);
}

void InputDevice::Free(void)
{
	SafeRelease(mKeyBoard);
	SafeRelease(mMouse);
	SafeRelease(mInputSDK);
}

