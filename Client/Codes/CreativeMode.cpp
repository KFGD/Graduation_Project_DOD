#include "stdafx.h"
#include "CreativeMode.h"

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

#include "KObject.h"

#include "WorldController.h"


//	Function
//	For: DearImgui

static bool Items_DisplayVectorGetter(void* data, int idx, const char** out_text)
{
	static char buff[128] = "";

	const vector<pair<_int, KObject*>>& displayList = *(vector<pair<_int, KObject*>>*)data;
	
	if (0 == displayList.size())
		return false;

	if (out_text)
	{
		const pair<_int, KObject*>& pair = displayList[idx];
		const KObject::Info& info = pair.second->GetInfo();
		sprintf_s(buff, "%s \t\t\t(%.3f, %.3f, %.3f)", pair.second->GetObjectTypeName(), info.Transform.Position.x, info.Transform.Position.y, info.Transform.Position.z);
		*out_text = buff;
	}
	return true;
}

CreativeMode::CreativeMode()
{	
}

void CreativeMode::Active(IWorldController* worldController)
{
	InitSampleData();
	worldController->ClearObjectList();
	worldController->SetUpObjectList(mObjectList);

	mbDisplayObjectFilter.fill(true);
	mDisplayObjectList.reserve(mObjectList.size());
	UpdateDisplayList();
}

void CreativeMode::InActive()
{
	ClearDisplayObjectList();
	ClearObjectList();
}

void CreativeMode::Update()
{
	UpdateDisplayObjectListUI();
	UpdateEditorUI();
}

void CreativeMode::UpdateDisplayObjectListUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("ObjectList", ImVec2(0, 250), true, 0);

	constexpr char* objectTypeName[Game::Type_End] = { "Player", "Bot", "Block" };

	ImGui::Text("[ObjectList]");

	ImGui::Text("Filter: ");
	ImGui::SameLine();

	_bool bNeedUpdate = false;
	for (_int i = 0; i < Game::Type_End; ++i)
	{
		const _bool curValue = mbDisplayObjectFilter[i];
		ImGui::Checkbox(objectTypeName[i], &mbDisplayObjectFilter[i]);
		ImGui::SameLine();
		if (curValue != mbDisplayObjectFilter[i])
			bNeedUpdate = true;
	}
	ImGui::NewLine();

	if (bNeedUpdate)
	{
		ClearDisplayObjectList();
		UpdateDisplayList();
	}

	ImGui::SetNextItemWidth(-1);
	ImGui::ListBox("", &mSelectedObjectListIndex, Items_DisplayVectorGetter, (void*)&mDisplayObjectList, mDisplayObjectList.size(), 8);

	ImGui::NewLine();
	ImGui::SameLine(ImGui::GetWindowWidth() - 70.f);
	if (ImGui::Button("Delete") && -1 != mSelectedObjectListIndex)
	{
		DISPLAY_PAIR& pair = mDisplayObjectList[mSelectedObjectListIndex];
		SafeRelease(pair.second);
		
		auto iter = mObjectList.begin();
		for (_int i = 0; i < pair.first; ++i)
			++iter;

		SafeRelease((*iter));
		mObjectList.erase(iter);

		ClearDisplayObjectList();
		UpdateDisplayList();

		mSelectedObjectListIndex = -1;
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
	
	//ImGui::Separator();
}

void CreativeMode::UpdateEditorUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("Editor", ImVec2(0, 250), true, 0);

	ImGui::Text("[Editor]");
	ImGui::Text("Cursor: %.3f, %.3f, %.3f", 1.f, 1.f, 1.f);

	ImGui::EndChild();
	ImGui::PopStyleVar();

}

void CreativeMode::UpdateObjectList()
{
}

void CreativeMode::UpdateDisplayList()
{
	_int flag = 0;
	for (_int i = 0; i < Game::Type_End; ++i)
		if (mbDisplayObjectFilter[i])
			flag |= 1 << i;

	for (_int i = 0; i < mObjectList.size(); ++i)
	{
		KObject* object = mObjectList[i];
		const KObject::Info& info = object->GetInfo();
		if (flag & (1 << info.Objecttype))
		{
			SafeAddRef(object);
			mDisplayObjectList.emplace_back(i, object);
		}
	}
}

void CreativeMode::ClearObjectList()
{
	for (KObject*& object : mObjectList)
		SafeRelease(object);
	mObjectList.clear();
}

void CreativeMode::ClearDisplayObjectList()
{
	for (DISPLAY_PAIR& pair : mDisplayObjectList)
		SafeRelease(pair.second);
	mDisplayObjectList.clear();
}

void CreativeMode::InitSampleData()
{
	mObjectList.emplace_back(KObject::Create(KObject::Info(Game::ObjectType::Player,
		KEngine::Transform(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), _vec3(0.f, 0.f, 0.f)))));

	constexpr _float gap = 0.5f;
	for (_int i = -4; i < -1; ++i)
		for (_int j = -4; j < -1; ++j)
			mObjectList.emplace_back(KObject::Create(KObject::Info(Game::ObjectType::Bot,
				KEngine::Transform(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), _vec3(i * gap, 0.f, j * gap)))));
}

CreativeMode * CreativeMode::Create()
{
	return new CreativeMode();
}

void CreativeMode::Free()
{
	ClearDisplayObjectList();
	ClearObjectList();
}
