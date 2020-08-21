#pragma once
#include "Base.h"

#include "Defines.h"

class KeyManager :
	public Base
{
public:
	static constexpr _ulong KEY_W = 0x00000001;
	static constexpr _ulong KEY_A = 0x00000002;
	static constexpr _ulong KEY_S = 0x00000004;
	static constexpr _ulong KEY_D = 0x00000008;

	static constexpr _ulong KEY_LBUTTON = 0x00000010;
	static constexpr _ulong KEY_RBUTTON = 0x00000020;

	DECLARE_SINGLETON(KeyManager)

private:
	explicit KeyManager() = default;
	virtual ~KeyManager() = default;
	
public:
	void	Update(const _double timeDelta);
	_bool	KeyDown(const _ulong key);
	_bool	KeyUp(const _ulong key);
	_bool	KeyPresseing(const _ulong key);

private:
	_ulong	mKeyBuf;
	_ulong	mKeyDown;
	_ulong	mKeyPressed;

public:
	virtual void	Free()	override;
};

