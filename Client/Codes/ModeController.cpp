#include "stdafx.h"
#include "ModeController.h"

////////////////////////////
//
//
//	Imgui
//
//
////////////////////////////
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx9.h>

#include "GameMode.h"
#include "CreativeMode.h"
#include "CameraController.h"
#include "FreeCamera.h"

IMPLEMENT_SINGLETON(ModeController)

ModeController::ModeController()
{
	mMode.fill(nullptr);
}

_bool ModeController::Ready(LPDIRECT3DDEVICE9 graphicDevice)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX9_Init(graphicDevice);

	mMode[Program::GAME] = GameMode::Create(graphicDevice);
	mMode[Program::CREATIVE] = CreativeMode::Create(graphicDevice);
	
	return true;
}

void ModeController::Update(const _double deltaTime, CameraController* cameraController)
{
	MappingCameraControllerToData(cameraController);
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//	Mode Controller
	ImGui::SetNextWindowPos(ImVec2(10.f, 10.f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(360.f, (_float)WINCY - 20.f));
	UpdateModeControllerUI(deltaTime);

	ImGui::SetNextWindowPos(ImVec2(400.f, 10.f), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300.f, 200.f));
	UpdateCameraControllerUI(cameraController);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//ImGui::ShowDemoWindow(&mIsWindowMode);

	ImGui::EndFrame();

	if (ImGui::IsKeyReleased(VK_CONTROL))
		mIsCameraLocking = !mIsCameraLocking;

	MappingDataToCameraController(cameraController);
}

void ModeController::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	mMode[mCurMode]->Render(graphicDevice);

	graphicDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	graphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	graphicDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	graphicDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	graphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	graphicDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
}

void ModeController::UpdateModeControllerUI(const _double deltaTime)
{
	typedef Program::Mode Mode;

	ImGui::Begin("Mode Controller", &mIsShowModeController, ImGuiWindowFlags_NoResize);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::NewLine();

	Mode preMode = mCurMode;
	//	Select mode
	{
		ImGui::Text("Select Mode: ");

		const Program::Mode preMode = mCurMode;
		
		ImGui::RadioButton("Game", (int*)&mCurMode, Mode::GAME);
		ImGui::SameLine();
		ImGui::RadioButton("CREATIVE", (int*)&mCurMode, Mode::CREATIVE);
	}

	if (preMode != mCurMode)
	{
		ChangeMode(mCurMode);
		mMode[preMode]->InActive();
		mMode[mCurMode]->Active();
	}

	//	Explain mode
	{
		switch (mCurMode)
		{
		case Mode::GAME:
			ImGui::Text("This mode can test and profile to the game.");
			break;

		case Mode::CREATIVE:
			ImGui::Text("This mode can edit the game scene.");
			break;
		}

		ImGui::NewLine();

	}

	//	Mode
	mMode[mCurMode]->Update(deltaTime);
	ImGui::End();
}

void ModeController::UpdateCameraControllerUI(CameraController* cameraController)
{
	ImGui::Begin("Camera Controller", &mIsShowCameraController);

	ImGui::Text("Camera Type: ");

	ImGui::RadioButton("Free", (int*)&mCurCameraType, CameraType::FREE_CAMERA);
	
	ImGui::SameLine();
	ImGui::RadioButton("Player", (int*)&mCurCameraType, CameraType::PLAYER_CAMERA);
	
	ImGui::Text("Locking: ");

	ImGui::SameLine();
	ImGui::Checkbox("##CameraLocking", &mIsCameraLocking);

	ImGui::NewLine();
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

	switch (mCurCameraType)
	{
	case CameraType::FREE_CAMERA:
	{
		ImGui::BeginChild("FreeCameraType", ImVec2(0, 100), true, 0);
		
		ImGui::Text("[Free Camera]");

		if (ImGui::Button("ResetRotation"))
		{
			FreeCamera*	freeCamera = static_cast<FreeCamera*>(cameraController->GetCamera(CameraType::FREE_CAMERA));
			freeCamera->ResetRotation();
		}

		ImGui::Text("MoveSpeed: ");
		
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50.f);
		ImGui::InputFloat("##FreeCameraMoveSpeed", &mFreeCameraMoveSpeed);

		ImGui::EndChild();
		
	}

	break;
	}

	ImGui::PopStyleVar();

	ImGui::End();
}

void ModeController::MappingDataToCameraController(CameraController * cameraController)
{
	cameraController->SetLocking(mIsCameraLocking);
	cameraController->ChangeCamera(mCurCameraType);

	switch (mCurCameraType)
	{
	case CameraType::FREE_CAMERA:
	{
		FreeCamera*	freeCamera = static_cast<FreeCamera*>(cameraController->GetCamera(CameraType::FREE_CAMERA));
		freeCamera->SetMoveSpeed(mFreeCameraMoveSpeed);
	}
		break;
	}

}

void ModeController::MappingCameraControllerToData(CameraController * cameraController)
{
	mIsCameraLocking = cameraController->GetLocking();
	mCurCameraType = cameraController->GetCurCameraType();

	switch (mCurCameraType)
	{
	case CameraType::FREE_CAMERA:
	{
		FreeCamera*	freeCamera = static_cast<FreeCamera*>(cameraController->GetCamera(CameraType::FREE_CAMERA));
		mFreeCameraMoveSpeed = freeCamera->GetMoveSpeed();
	}
		break;
	}

}

void ModeController::ChangeMode(const Program::Mode mode)
{
	GameMode*	gameMode = static_cast<GameMode*>(mMode[Program::Mode::GAME]);
	CreativeMode*	creativeMode = static_cast<CreativeMode*>(mMode[Program::Mode::CREATIVE]);

	if (Program::Mode::CREATIVE == mode)
	{
		
	}
	else
	{
		const NaviMeshData*	naviMeshData = creativeMode->GetNaviMeshData();
		gameMode->SetNaviMeshData(naviMeshData);

		const vector<KObject*>&	objectList = creativeMode->GetObjectList();
		gameMode->SetObjectList(objectList);
	}
}

void ModeController::Free()
{
	for (Mode* mode : mMode)
		SafeRelease(mode);
	mMode.fill(nullptr);


	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
