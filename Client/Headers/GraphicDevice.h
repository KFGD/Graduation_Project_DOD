#pragma once
#include "Base.h"
#include "Defines.h"

class GraphicDevice :
	public Base
{
	DECLARE_SINGLETON(GraphicDevice)

public:
	enum class WINMODE { FULL, WIN };

public:
	explicit GraphicDevice();
	virtual ~GraphicDevice() = default;

public:
	LPDIRECT3DDEVICE9 GetGraphicDevice() { return mGraphicDev; }
	_int	GetApproxNumBones() const { return mApproxNumBones; }
	_bool ReadyGraphicDevice(const HWND windowHandle, const WINMODE mode, const UINT sizeX, const UINT sizeY, LPDIRECT3DDEVICE9& graphicDev);

private:
	LPDIRECT3D9			mSDK = nullptr;
	LPDIRECT3DDEVICE9	mGraphicDev = nullptr;

private:
	_int	mApproxNumBones = 0;

public:
	virtual void	Free() override;

};

