#include "stdafx.h"
#include "MainApp.h"

////////////////////////////
//
//
//	Imgui
//
//
////////////////////////////
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"

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

MainApp::MainApp()
	: mGraphicDeviceSys(GraphicDevice::GetInstance())
{
	SafeAddRef(mGraphicDeviceSys);

	mWorlds.fill(nullptr);
}

bool MainApp::Initialize()
{
	if (false == ReadySystem(g_hWnd, mIsWindowMode, WINCX, WINCY))
		return false;
	
	if (false == ReadyWorld())
		return false;

	//SetUpLighting();
	mGraphicDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//mGraphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	//
	//mGraphicDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	//mGraphicDevice->SetRenderState(D3DRS_DITHERENABLE, TRUE);
	//mGraphicDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	//mGraphicDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	//mGraphicDevice->SetRenderState(D3DRS_AMBIENT, 0x33333333);
	//mGraphicDevice->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	//mGraphicDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	//mGraphicDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	if (false == ReadyImGUI())
		return false;


	return true;
}

int MainApp::Update(const double tiemDelta)
{
	if (nullptr == mCurWorld)
		return -1;

	mCurWorld->Update(tiemDelta);
	UpdateImGUI();
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

	mCurWorld->Render();
	RenderImGUI();

	if (FAILED(mGraphicDevice->EndScene()))
		return false;

	if (FAILED(mGraphicDevice->Present(nullptr, nullptr, 0, nullptr)))
		return false;

	return true;
}

_bool MainApp::UpdateImGUI()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	
	
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (mIsWindowMode)
		ImGui::ShowDemoWindow(&mIsWindowMode);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &mIsWindowMode);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &mIsShowAnotherWindow);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&ImVec4(0.45f, 0.55f, 0.60f, 1.00f)); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (mIsShowAnotherWindow)
	{
		ImGui::Begin("Another Window", &mIsShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			mIsShowAnotherWindow = false;
		ImGui::End();
	}

	ImGui::EndFrame();

	return _bool();
}

_bool MainApp::RenderImGUI()
{
	mGraphicDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	mGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	mGraphicDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	mGraphicDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	mGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	mGraphicDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
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
	mWorlds[OOD] = World_Object::Create(mGraphicDevice);

	mCurWorld = mWorlds[000];
	return true;
}

_bool MainApp::SetUpLighting()
{
	_vec3 lightDir = *D3DXVec3Normalize(&lightDir, &_vec3(1.f, -1.f, 1.f));
	D3DLIGHT9 lightDesc;

	::ZeroMemory(&lightDesc, sizeof(D3DLIGHT9));
	lightDesc.Type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;
	lightDesc.Direction = lightDir;
	lightDesc.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	lightDesc.Ambient = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	lightDesc.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	mGraphicDevice->SetLight(0, &lightDesc);
	mGraphicDevice->LightEnable(0, TRUE);
	mGraphicDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	D3DMATERIAL9 material;
	::ZeroMemory(&material, sizeof(D3DMATERIAL9));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.f);
	material.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	material.Power = 1.f;
	mGraphicDevice->SetMaterial(&material);

	return true;
}

_bool MainApp::ReadyImGUI()
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
	ImGui_ImplDX9_Init(mGraphicDevice);

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
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	mCurWorld = nullptr;

	for (World* world : mWorlds)
		if(nullptr != world)
			world->Clear();

	for (World* world : mWorlds)
		SafeRelease(world);
	mWorlds.fill(nullptr);

	SafeRelease(mGraphicDeviceSys);
	SafeRelease(mGraphicDevice);

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

