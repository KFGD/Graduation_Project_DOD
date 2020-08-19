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

#include "CreativeMode.h"

IMPLEMENT_SINGLETON(ModeController)

ModeController::ModeController()
{
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

	mCreativeMode = CreativeMode::Create();
	if (nullptr == mCreativeMode)
		return false;

	return true;
}

void ModeController::Update()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	//	Mode Controller
	ImGui::SetNextWindowPos(ImVec2(10.f, 10.f));
	UpdateModeController();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (mIsWindowMode)
		ImGui::ShowDemoWindow(&mIsWindowMode);

	//// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	//{
	//	static float f = 0.0f;
	//	static int counter = 0;

	//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//	ImGui::Checkbox("Demo Window", &mIsWindowMode);      // Edit bools storing our window open/close state
	//	ImGui::Checkbox("Another Window", &mIsShowAnotherWindow);

	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//	ImGui::ColorEdit3("clear color", (float*)&ImVec4(0.45f, 0.55f, 0.60f, 1.00f)); // Edit 3 floats representing a color

	//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//		counter++;
	//	ImGui::SameLine();
	//	ImGui::Text("counter = %d", counter);

	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//	ImGui::End();
	//}

	//// 3. Show another simple window.
	//if (mIsShowAnotherWindow)
	//{
	//	ImGui::Begin("Another Window", &mIsShowAnotherWindow);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		mIsShowAnotherWindow = false;
	//	ImGui::End();
	//}

	ImGui::EndFrame();
}

void ModeController::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	graphicDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	graphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	graphicDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	graphicDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	graphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	graphicDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
}

void ModeController::UpdateModeController()
{
	typedef Program::Mode Mode;
	
	ImGui::Begin("Mode Controller", &mbShowWindow, ImGuiWindowFlags_AlwaysAutoResize);
	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
	ImGui::NewLine();

	Mode preMode = mCurMode;
	//	Select mode
	{
		ImGui::Text("Select Mode: ");

		ImGui::RadioButton("Game", (int*)&mCurMode, Mode::GAME);
		ImGui::SameLine();
		ImGui::RadioButton("CREATIVE", (int*)&mCurMode, Mode::CREATIVE);
	}

	if (preMode != mCurMode)
	{
		if (Mode::GAME == mCurMode)
		{
			mCreativeMode->InActive();
		}
		else if (Mode::CREATIVE == mCurMode)
		{
			mCreativeMode->Active();
		}
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
	{
		switch (mCurMode)
		{
		case Mode::GAME:
			break;

		case Mode::CREATIVE:
			mCreativeMode->Update();
			break;
		}
	}

	ImGui::End();

}

void ModeController::Free()
{
	SafeRelease(mCreativeMode);

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
