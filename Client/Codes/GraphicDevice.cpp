#include "stdafx.h"
#include "GraphicDevice.h"

IMPLEMENT_SINGLETON(GraphicDevice)

GraphicDevice::GraphicDevice()
{
}

_bool GraphicDevice::ReadyGraphicDevice(const HWND windowHandle, const WINMODE mode, const UINT sizeX, const UINT sizeY, LPDIRECT3DDEVICE9 & graphicDev)
{
	mSDK = Direct3DCreate9(D3D_SDK_VERSION);

	if (nullptr == mSDK)
		return false;

	D3DCAPS9			DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	if (FAILED(mSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
		return false;

	//	Hardware Skinning: Matrix Palette
	mApproxNumBones = DeviceCaps.MaxVertexShaderConst / 4;

	unsigned long		uFlag = 0;

	if (DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		uFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;

	else
		uFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE;

	D3DPRESENT_PARAMETERS			presentParam;
	ZeroMemory(&presentParam, sizeof(D3DPRESENT_PARAMETERS));

	presentParam.BackBufferWidth = sizeX;
	presentParam.BackBufferHeight = sizeY;
	presentParam.BackBufferFormat = D3DFMT_A8R8G8B8;
	presentParam.BackBufferCount = 1;

	presentParam.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentParam.MultiSampleQuality = 0;

	presentParam.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParam.hDeviceWindow = windowHandle;

	presentParam.Windowed = (mode == WINMODE::WIN ? TRUE : FALSE);
	presentParam.EnableAutoDepthStencil = TRUE;
	presentParam.AutoDepthStencilFormat = D3DFMT_D24S8;

	presentParam.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	presentParam.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(mSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, windowHandle, uFlag, &presentParam, &mGraphicDev)))
		return false;

	graphicDev = mGraphicDev;
	SafeAddRef(mGraphicDev);

	return true;
}

void GraphicDevice::Free()
{
	unsigned long refCnt = 0;

	if (refCnt = SafeRelease(mGraphicDev))
		MSG_BOX("mGraphicDev Release Failed!");
	if (refCnt = SafeRelease(mSDK))
		MSG_BOX("mSDK Release FAiled");
}
