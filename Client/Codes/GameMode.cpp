#include "stdafx.h"
#include "GameMode.h"

#include "KObject.h"
#include "World_Object.h"
#include "World_Data.h"

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

GameMode::GameMode()
{
	mWorlds.fill(nullptr);
}

void GameMode::Active()
{
	
}

void GameMode::InActive()
{
	ClearObjectList();
}

void GameMode::Update_UI(const _double timeDelta)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("WorldType", ImVec2(0, 40), true, 0);

	ImGui::Text("[World Type]");

	ImGui::SameLine();
	if (ImGui::RadioButton("Object##World_Object", mCurWorldType == Game::Object_Oriented))
		ChangeWorldType(Game::Object_Oriented);
	

	ImGui::SameLine();
	if (ImGui::RadioButton("Data##World_Data", mCurWorldType == Game::Data_Oriented))
		ChangeWorldType(Game::Data_Oriented);


	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void GameMode::Update_Object(const _double timeDelta)
{
	mWorlds[mCurWorldType]->Update(timeDelta);
}

void GameMode::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	mWorlds[mCurWorldType]->Render();
}

void GameMode::SetObjectList(const vector<KObject*>& objectList)
{
	for (const KObject* object : objectList)
	{
		KObject* newObject = KObject::Create(object->GetInfo());
		mObjectList.emplace_back(newObject);
	}

	mWorlds[mCurWorldType]->Clear();
	mWorlds[mCurWorldType]->SetUpObjectList(mObjectList);
}

void GameMode::SetNaviMeshData(const NaviMeshData * naviMeshData)
{
	mWorlds[mCurWorldType]->SetUpNaviMesh(naviMeshData);
}

_bool GameMode::Initialize(LPDIRECT3DDEVICE9 graphicDevice)
{
	mWorlds[Game::Object_Oriented] = World_Object::Create(graphicDevice);
	mWorlds[Game::Data_Oriented] = World_Data::Create(graphicDevice);

	mCurWorldType = Game::Object_Oriented;

	return true;
}

void GameMode::ChangeWorldType(Game::WorldType nextWorldType)
{
	if (mCurWorldType == nextWorldType)
		return;
	
	mWorlds[mCurWorldType]->Clear();

	mCurWorldType = nextWorldType;
	mWorlds[mCurWorldType]->SetUpObjectList(mObjectList);
}

void GameMode::ClearObjectList()
{
	for (KObject*& object : mObjectList)
		SafeRelease(object);
	mObjectList.clear();
}

GameMode * GameMode::Create(LPDIRECT3DDEVICE9 graphicDevice)
{
	GameMode*	pInstance = new GameMode();

	if (false == pInstance->Initialize(graphicDevice))
	{
		MSG_BOX("GameMode Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void GameMode::Free()
{
	for (World*& world : mWorlds)
	{
		if(nullptr != world)
			world->Clear();
	}

	for (World*& world : mWorlds)
		SafeRelease(world);
	mWorlds.fill(nullptr);

	ClearObjectList();
	Mode::Free();
}


