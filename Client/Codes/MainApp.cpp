#include "stdafx.h"
#include "MainApp.h"

//	System
#include "GraphicDevice.h"
#include "TimeManager.h"
#include "FrameManager.h"

//	Core
#include "World.h"
#include "PipeLine.h"

///////////////////////////
//
//	OOD
//
//////////////////////////
#include "World_Object.h"
#include "ComponentManager_Object.h"

#include "ModeController.h"

MainApp::MainApp()
	: mGraphicDeviceSys(GraphicDevice::GetInstance())
	, mModeController(ModeController::GetInstance())
{
	SafeAddRef(mGraphicDeviceSys);
	SafeAddRef(mModeController);

	mWorlds.fill(nullptr);
}

_bool MainApp::ChangeWorld(const Game::WorldType worldType)
{
	mCurMode = worldType;
	return true;
}

_bool MainApp::SetUpObjectList(const vector<KObject*>& objectList)
{
	mWorlds[mCurMode]->SetUpObjectList(objectList);
	return true;
}

_bool MainApp::ClearObjectList()
{
	mWorlds[mCurMode]->Clear();
	return true;
}

int MainApp::Update(const double tiemDelta)
{
	if (nullptr == mWorlds[mCurMode])
		return -1;

	mWorlds[mCurMode]->Update(tiemDelta);
	mModeController->Update(this);

	return 0;
}

bool MainApp::Render()
{
	if (nullptr == mGraphicDevice)
		return false;

	if (FAILED(mGraphicDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0)))
		return false;

	if (FAILED(mGraphicDevice->BeginScene()))
		return false;

	mWorlds[mCurMode]->Render();
	mModeController->Render(mGraphicDevice);

	if (FAILED(mGraphicDevice->EndScene()))
		return false;

	if (FAILED(mGraphicDevice->Present(nullptr, nullptr, 0, nullptr)))
		return false;

	return true;
}

bool MainApp::Initialize()
{
	if (false == ReadySystem(g_hWnd, true, WINCX, WINCY))
		return false;

	if (false == ReadyWorld())
		return false;

	mGraphicDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (false == mModeController->Ready(mGraphicDevice))
		return false;

	return true;
}

bool MainApp::ReadySystem(const HWND hWnd, const bool bWinMode, const UINT sizeX, const UINT sizeY)
{
	if (false == mGraphicDeviceSys->ReadyGraphicDevice(hWnd, (bWinMode == true ? GraphicDevice::WINMODE::WIN : GraphicDevice::WINMODE::FULL), sizeX, sizeY, mGraphicDevice))
		return false;
	
	return true;
}

bool MainApp::ReadyWorld()
{
	mWorlds[Game::Object_Oriented] = World_Object::Create(mGraphicDevice);

	mCurMode = Game::Object_Oriented;

	return true;
}


MainApp * MainApp::Create()
{
	MainApp*	pInstance = new MainApp();
	if (false == pInstance->Initialize())
	{
		MSG_BOX("MainApp Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void MainApp::Free()
{
	for (World* world : mWorlds)
		SafeRelease(world);
	mWorlds.fill(nullptr);

	SafeRelease(mModeController);
	
	SafeRelease(mGraphicDeviceSys);
	SafeRelease(mGraphicDevice);
	
	//	Destroy: UI
	if (0 != ModeController::DestroyInstance())
		MSG_BOX("UIManager_Profiler Release Failed!");

	//	Destroy: OOD
	if (0 != ComponentManager_Object::DestroyInstance())
		MSG_BOX("ComponentManager_Object Release Failed!");

	//	Destroy: Core
	if (0 != PipeLine::DestroyInstance())
		MSG_BOX("PipeLine Release Failed!");

	// Destroy: System
	if (0 != TimeManager::DestroyInstance())
		MSG_BOX("TimeManager Release Failed!");

	if (0 != FrameManager::DestroyInstance())
		MSG_BOX("FrameManager Release Failed");

	if (0 != GraphicDevice::DestroyInstance())
		MSG_BOX("GraphicDevice Release Failed");

}

