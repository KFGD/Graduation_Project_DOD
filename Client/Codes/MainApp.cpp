#include "stdafx.h"
#include "MainApp.h"

//	System
#include "GraphicDevice.h"
#include "InputDevice.h"
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
#include "CameraController.h"
#include "KeyManager.h"

MainApp::MainApp()
	: mGraphicDeviceSys(GraphicDevice::GetInstance())
	, mInputDevice(InputDevice::GetInstance())
	, mModeController(ModeController::GetInstance())
	, mCameraController(CameraController::GetInstance())
	, mKeyManager(KeyManager::GetInstance())
{
	SafeAddRef(mGraphicDeviceSys);
	SafeAddRef(mInputDevice);
	SafeAddRef(mModeController);
	SafeAddRef(mCameraController);
	SafeAddRef(mKeyManager);

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

	mInputDevice->SetInputDev();
	mKeyManager->Update(tiemDelta);

	mCameraController->Update(tiemDelta);

	if (Program::GAME == mModeController->GetCurProgramMode())
		mWorlds[mCurMode]->Update(tiemDelta);
	
	mModeController->Update(this, mCameraController);

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
	if (Program::GAME == mModeController->GetCurProgramMode())
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

	if (false == mCameraController->Ready())
		return false;

	mGraphicDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	if (false == mModeController->Ready(mGraphicDevice))
		return false;



	//_matrix viewMatrix = *D3DXMatrixLookAtLH(&viewMatrix, &_vec3(0.f, 3.f, -3.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f));
	_matrix projMatrix = *D3DXMatrixPerspectiveFovLH(&projMatrix, D3DXToRadian(60.f), (_float)WINCX / (_float)WINCY, 0.01f, 1000.f);
	PipeLine::GetInstance()->SetTransform(D3DTS_PROJECTION, projMatrix);

	return true;
}

bool MainApp::ReadySystem(const HWND hWnd, const bool bWinMode, const UINT sizeX, const UINT sizeY)
{
	if (false == mGraphicDeviceSys->ReadyGraphicDevice(hWnd, (bWinMode == true ? GraphicDevice::WINMODE::WIN : GraphicDevice::WINMODE::FULL), sizeX, sizeY, mGraphicDevice))
		return false;

	if (false == mInputDevice->Ready(g_hInst, hWnd))
		return false;

	if (false == PipeLine::GetInstance()->Ready(mGraphicDevice))
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
		if(nullptr != world)
			world->Clear();

	for (World* world : mWorlds)
		SafeRelease(world);
	mWorlds.fill(nullptr);

	SafeRelease(mKeyManager);
	SafeRelease(mCameraController);
	SafeRelease(mModeController);
	
	SafeRelease(mInputDevice);

	SafeRelease(mGraphicDeviceSys);
	SafeRelease(mGraphicDevice);
	
	//	Destroy: Key
	if(0 != KeyManager::DestroyInstance())
		MSG_BOX("KeyManager Release Failed!");

	//	Destroy: Camera
	if (0 != CameraController::DestroyInstance())
		MSG_BOX("CameraController Release Failed!");

	//	Destroy: UI
	if (0 != ModeController::DestroyInstance())
		MSG_BOX("ModeController Release Failed!");

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

	if (0 != InputDevice::DestroyInstance())
		MSG_BOX("InputDevice Release Failed");

	if (0 != GraphicDevice::DestroyInstance())
		MSG_BOX("GraphicDevice Release Failed");

}

