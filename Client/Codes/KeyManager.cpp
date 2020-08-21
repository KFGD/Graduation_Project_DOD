#include "stdafx.h"
#include "KeyManager.h"

#include "InputDevice.h"

IMPLEMENT_SINGLETON(KeyManager)

void KeyManager::Update(const _double timeDelta)
{
	InputDevice* inputDevice = InputDevice::GetInstance();
	mKeyBuf = 0;
	if (0x80 & inputDevice->GetDIKeyState(DIK_W))
		mKeyBuf |= KEY_W;
	if (0x80 & inputDevice->GetDIKeyState(DIK_A))
		mKeyBuf |= KEY_A;
	if (0x80 & inputDevice->GetDIKeyState(DIK_S))
		mKeyBuf |= KEY_S;
	if (0x80 & inputDevice->GetDIKeyState(DIK_D))
		mKeyBuf |= KEY_D;


	if (0x80 & inputDevice->GetDIMouseState(InputDevice::MOUSEKEYSTATE::DIM_LBUTTON))
		mKeyBuf |= KEY_LBUTTON;
	if (0x80 & inputDevice->GetDIMouseState(InputDevice::MOUSEKEYSTATE::DIM_RBUTTON))
		mKeyBuf |= KEY_RBUTTON;
}

_bool KeyManager::KeyDown(const _ulong key)
{
	if (!(mKeyDown & key) && (mKeyBuf & key))
	{
		mKeyDown |= key;
		return true;
	}

	if ((mKeyDown & key) && !(mKeyBuf & key))
	{
		mKeyDown ^= key;
		return false;
	}

	return false;
}

_bool KeyManager::KeyUp(const _ulong key)
{
	if ((mKeyPressed & key) && !(mKeyBuf & key))
	{
		mKeyPressed ^= key;
		return true;
	}

	// 이전에 누른적 없고 현재 눌렀다면 FALSE
	if (!(mKeyPressed & key) && (mKeyBuf & key))
	{
		mKeyPressed |= key;
		return false;
	}

	return false;
}

_bool KeyManager::KeyPresseing(const _ulong key)
{
	if (mKeyBuf & key)
		return true;

	return false;
}

void KeyManager::Free()
{
}
