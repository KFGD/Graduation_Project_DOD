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

////////////////////////////
//
//
//	Windows
//
//
////////////////////////////
#include <commdlg.h>
#include <PathCch.h>

#include "KeyManager.h"
#include "KObject.h"
#include "DynamicMesh_Object.h"
#include "StaticMesh_Object.h"
#include "WorldController.h"
#include "PipeLine.h"
#include "Shader.h"
#include "NaviMeshData.h"

//	Function
//	For: DearImgui

static bool Items_DisplayVectorGetter(void* data, int idx, const char** out_text)
{
	static char buff[128] = "";

	const vector<KObject*>& objectList = *(vector<KObject*>*)data;

	if (0 == objectList.size())
		return false;

	if (out_text)
	{
		KObject* object = objectList[idx];
		const KObject::Info& info = object->GetInfo();
		sprintf_s(buff, "%s \t\t\t(%.3f, %.3f, %.3f)", object->GetObjectTypeName(), info.Transform.Position.x, info.Transform.Position.y, info.Transform.Position.z);
		*out_text = buff;
	}
	return true;
}

CreativeMode::CreativeMode()
{
}

void CreativeMode::Active(IWorldController* worldController)
{
	mSelectedObjectListIndex = -1;
	//InitSampleData();
}

void CreativeMode::InActive(IWorldController* worldController)
{
	ClearObjectList();
}

void CreativeMode::Update(IWorldController* worldController)
{
	UpdateSelectTargetMode();
	UpdateFileUI();

	if (TargetMode::OBJECT_MODE == mCurMode)
	{
		MappingObjectToObjectUI(mSelectedObjectListIndex);

		UpdateDisplayObjectListUI(worldController);
		UpdateCreateUI();

		LogicObject();

		MappingEditUIToEditObject(mSelectedObjectListIndex);
	}
	else
	{
		MappingNaviMeshDataToNaviMeshUI();
		
		UpdateNaviMeshDataUI();

		if (mIsNaviMeshCreate)
			LogicNaviMeshCreateMode();
		else
			LogicNaviMeshEditMode();

		MappingNaviMeshUIToNaviMeshData();
	}
}

void CreativeMode::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
	KObject* selectedObj = nullptr;
	list<KObject*>	playerList;
	list<KObject*>	botList;
	vector<KObject*>	blockList;

	blockList.reserve(mObjectList.size());

	for (_int i = 0; i < mObjectList.size(); ++i)
	{
		KObject*& object = mObjectList[i];

		if (i == mSelectedObjectListIndex)
		{
			selectedObj = object;
			continue;
		}

		switch (object->GetInfo().Objecttype)
		{
		case Game::Player:
			playerList.emplace_back(object);
			break;
		case Game::Bot:
			botList.emplace_back(object);
			break;
		case Game::Block:
			blockList.emplace_back(object);
			break;
		default:
			break;
		}
	}
	PipeLine* pipeLine = PipeLine::GetInstance();
	const _matrix matVP = pipeLine->GetTransform(D3DTS_VIEW) * pipeLine->GetTransform(D3DTS_PROJECTION);

	mMeshShader->SetValue("gMatVP", &matVP, sizeof(_matrix));
	mMeshShader->BeginShader(nullptr);
	mMeshShader->BeginPass(0);

	for (KObject*& object : playerList)
		RenderSkinnedMesh(mMeshShader, object, mPlayerMesh);

	for (KObject*& object : botList)
		RenderSkinnedMesh(mMeshShader, object, mBotMesh);

	mMeshShader->EndPass();
	mMeshShader->EndShader();

	RenderInstanceMesh(graphicDevice, blockList, mBlockMesh);

	//graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	mSelectedMeshShader->SetValue("gMatVP", &matVP, sizeof(_matrix));
	mSelectedMeshShader->BeginShader(nullptr);
	mSelectedMeshShader->BeginPass(0);
	if (nullptr != selectedObj)
	{
		switch (selectedObj->GetInfo().Objecttype)
		{
		case Game::Player:
			RenderSkinnedMesh(mSelectedMeshShader, selectedObj, mPlayerMesh);
			break;

		case Game::Bot:
			RenderSkinnedMesh(mSelectedMeshShader, selectedObj, mBotMesh);
			break;

		case Game::Block:
			mSelectedMeshShader->SetValue("gMatWorld", &selectedObj->GetWorldMatrix(), sizeof(_matrix));
			mSelectedMeshShader->SetTexture("gDiffuseTexture", mBlockMesh->GetTexutre(0));
			mSelectedMeshShader->CommitChanges();
			mBlockMesh->GetMesh()->DrawSubset(0);
			break;
		}
	}

	mSelectedMeshShader->EndPass();
	mSelectedMeshShader->EndShader();

	mNaviMeshData->Render(graphicDevice);

	//graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

_bool CreativeMode::PickingObject(_int& selectedIndex, _vec3& hitWorldPos)
{
	POINT mouse;
	::GetCursorPos(&mouse);
	::ScreenToClient(g_hWnd, &mouse);

	PipeLine::RayCast worldRayCast = PipeLine::GetInstance()->ConvertScreenPosToWorldRayCast(_vec2((_float)mouse.x, (_float)mouse.y));
	_float nearestDis = MAX_FLOAT;
	selectedIndex = -1;

	for (_int i = 0; i < mObjectList.size(); ++i)
	{
		KObject*& object = mObjectList[i];
		const _matrix matInverseWorld = object->GetWorldInverseMatrix();

		_vec3 localRayPos, localRayDir;
		D3DXVec3TransformCoord(&localRayPos, &worldRayCast.RayPos, &matInverseWorld);
		D3DXVec3TransformNormal(&localRayDir, &worldRayCast.RayDir, &matInverseWorld);

		BOOL	bHit = false;
		_float	hitDis = 0.0;

		switch (object->GetInfo().Objecttype)
		{
		case Game::Player:
		{
			for (_int j = 0; j < mPlayerMesh->GetMeshContinerSize(); ++j)
			{
				D3DXMESHCONTAINER_DERIVED* meshContainer = mPlayerMesh->GetMeshContainer(j);
				LPD3DXMESH mesh = meshContainer->pSoftwareMesh;
				D3DXIntersect(mesh, &localRayPos, &localRayDir, &bHit, NULL, NULL, NULL, &hitDis, NULL, NULL);
				if (bHit)
				{
					if (hitDis < nearestDis)
					{
						selectedIndex = i;
						nearestDis = hitDis;
					}
				}
			}
		}
		break;

		case Game::Bot:
		{
			for (_int j = 0; j < mBotMesh->GetMeshContinerSize(); ++j)
			{
				D3DXMESHCONTAINER_DERIVED* meshContainer = mBotMesh->GetMeshContainer(j);
				LPD3DXMESH mesh = meshContainer->pSoftwareMesh;
				D3DXIntersect(mesh, &localRayPos, &localRayDir, &bHit, NULL, NULL, NULL, &hitDis, NULL, NULL);
				if (bHit)
				{
					if (hitDis < nearestDis)
					{
						selectedIndex = i;
						nearestDis = hitDis;
					}
				}
			}
		}
		break;

		case Game::Block:
		{
			LPD3DXMESH mesh = mBlockMesh->GetMesh();
			D3DXIntersect(mesh, &localRayPos, &localRayDir, &bHit, NULL, NULL, NULL, &hitDis, NULL, NULL);
			if (bHit)
			{
				if (hitDis < nearestDis)
				{
					selectedIndex = i;
					nearestDis = hitDis;
				}
			}
		}
		break;

		}
	}

	const _bool isHit = nearestDis != MAX_FLOAT;

	if (isHit)
		hitWorldPos = worldRayCast.RayPos + worldRayCast.RayDir * nearestDis;

	return isHit;
}

void CreativeMode::UpdateSelectTargetMode()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("TargetMode", ImVec2(0, 40), true, 0);

	ImGui::Text("[Target Mode]");

	ImGui::SameLine();
	if (ImGui::RadioButton("Object##ObjectCreate", mCurMode == TargetMode::OBJECT_MODE))
		mCurMode = OBJECT_MODE;

	ImGui::SameLine();
	if (ImGui::RadioButton("NaviMesh##NaviMeshCreate", mCurMode == TargetMode::NAVI_MODE))
		mCurMode = NAVI_MODE;


	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void CreativeMode::UpdateFileUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("FileSelector", ImVec2(0, 60), true, 0);

	ImGui::Text("[File Selector]");

	ImGui::Text("Path: ");

	ImGui::SameLine();

	ImGui::SetNextItemWidth(150);
	ImGui::InputText("##InputFilePath", mFilePath, sizeof(mFilePath), ImGuiInputTextFlags_ReadOnly);

	ImGui::SameLine(ImGui::GetWindowWidth() - 100.f);
	if (ImGui::Button("Open##FileSelector"))
	{
		// File Select
		_tchar initFilePath[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, initFilePath);
		PathCchRemoveFileSpec(initFilePath, sizeof(initFilePath));
		lstrcat(initFilePath, L"\\Data");

		_tchar selectFilePath[MAX_PATH] = L"";

		OPENFILENAME ofn;
		::ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFilter = L"All File(*.*)\0*.*\0Data File(.data)\0*.data;*.doc\0";
		ofn.lpstrFile = selectFilePath;
		ofn.nMaxFile = 256;
		ofn.lpstrInitialDir = initFilePath;
		if (0 != GetOpenFileName(&ofn))
		{
			ClearObjectList();

			HANDLE hFile = CreateFile(selectFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			DWORD dwBytes = 0;

			_size_t size = 0;
			ReadFile(hFile, &size, sizeof(_size_t), &dwBytes, nullptr);

			vector<KObject::Info> infoList(size);
			for (_int i = 0; i < size; ++i)
				ReadFile(hFile, &infoList[i], sizeof(KObject::Info), &dwBytes, nullptr);

			CloseHandle(hFile);

			mObjectList.reserve(size);
			for (_int i = 0; i < size; ++i)
				mObjectList.emplace_back(KObject::Create(infoList[i]));

			WideCharToMultiByte(CP_ACP, 0, selectFilePath, -1, mFilePath, MAX_PATH, 0, 0);

		}
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 50.f);
	if (ImGui::Button("Save##FileSelector"))
	{
		//	Save File
		_tchar initFilePath[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, initFilePath);
		PathCchRemoveFileSpec(initFilePath, sizeof(initFilePath));
		lstrcat(initFilePath, L"\\Data");

		_tchar selectFilePath[MAX_PATH] = L"";

		OPENFILENAME ofn;
		::ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFilter = L"Data File(.data)\0*.data\0All File(*.*)\0*.*;*.doc\0";
		ofn.lpstrFile = selectFilePath;
		ofn.nMaxFile = 256;
		ofn.lpstrInitialDir = initFilePath;
		if (0 != GetSaveFileName(&ofn))
		{
			const _size_t size = mObjectList.size();

			HANDLE hFile = CreateFile(selectFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

			DWORD dwBytes = 0;
			WriteFile(hFile, &size, sizeof(_size_t), &dwBytes, nullptr);

			for (_int i = 0; i < mObjectList.size(); ++i)
			{
				WriteFile(hFile, &mObjectList[i]->GetInfo(), sizeof(KObject::Info), &dwBytes, nullptr);
			}

			CloseHandle(hFile);

		}

	}


	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void CreativeMode::UpdateDisplayObjectListUI(IWorldController* worldController)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("ObjectList", ImVec2(0, 310), true, 0);

	constexpr char* objectTypeName[Game::Type_End] = { "Player", "Bot", "Block" };

	ImGui::Text("[ObjectList]");

	ImGui::SetNextItemWidth(-1);
	if (ImGui::ListBox("##ObjectList", &mSelectedObjectListIndex, Items_DisplayVectorGetter, (void*)&mObjectList, (_int)mObjectList.size(), 8))
		MappingObjectToObjectUI(mSelectedObjectListIndex);

	ImGui::NewLine();
	
	ImGui::Text("Scale:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat3("EditScale", mEditScale);
	
	ImGui::Text("Rotation:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat3("EditRotation", mEditRotation);
	
	ImGui::Text("Position:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat3("EdiotPosition", mEditPosition);


	ImGui::Text("ObjectType:");

	ImGui::SameLine();
	if (ImGui::RadioButton("Player##Edit", Game::Player == mEditObjectType))
		mEditObjectType = Game::Player;

	ImGui::SameLine();
	if (ImGui::RadioButton("Bot##Edit", Game::Bot == mEditObjectType))
		mEditObjectType = Game::Bot;


	ImGui::SameLine();
	if (ImGui::RadioButton("Block##Edit", Game::Block == mEditObjectType))
		mEditObjectType = Game::Block;

	ImGui::NewLine();
	ImGui::SameLine(ImGui::GetWindowWidth() - 70.f);
	if (ImGui::Button("Delete") && -1 != mSelectedObjectListIndex)
	{
		auto iter = mObjectList.begin();
		for (_int i = 0; i < mSelectedObjectListIndex; ++i)
			++iter;

		SafeRelease((*iter));
		mObjectList.erase(iter);

		mSelectedObjectListIndex = -1;

	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

	//ImGui::Separator();
}

void CreativeMode::UpdateCreateUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("Create", ImVec2(0, 150), true, 0);

	ImGui::Text("[Editor]");

	ImGui::Text("Scale:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat3("CreateScale", mCreateScale);

	ImGui::Text("Rotation:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat3("CreateRotation", mCreateRotation);

	ImGui::Text("Position:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat3("CreatePosition", mCreatePos);

	ImGui::Text("ObjectType:");

	ImGui::SameLine();
	if (ImGui::RadioButton("Player##Create", Game::Player == mCreateObjectType))
		mCreateObjectType = Game::Player;

	ImGui::SameLine();
	if (ImGui::RadioButton("Bot##Create", Game::Bot == mCreateObjectType))
		mCreateObjectType = Game::Bot;

	ImGui::SameLine();
	if (ImGui::RadioButton("Block##Create", Game::Block == mCreateObjectType))
		mCreateObjectType = Game::Block;

	ImGui::SameLine(ImGui::GetWindowWidth() - 40);
	if (ImGui::Button("Add"))
		CreateObject();

	ImGui::Text("Gap: ");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputFloat("##MultiAddGap", &mMultiCreateGap);

	ImGui::SameLine();
	ImGui::Text("Count: ");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(50.f);
	ImGui::InputInt("##MultiAddCount", &mMultiCreateCount, 0, 0, 0);

	ImGui::SameLine(ImGui::GetWindowWidth() - 75);
	if (ImGui::Button("MultiAdd"))
	{
		const _int begin = mMultiCreateCount * -1;
		const _int end = mMultiCreateCount;
		for (_int i = begin; i < end; ++i)
		{
			for (_int j = begin; j < end; ++j)
			{
				KObject::Info info(mCreateObjectType, KEngine::Transform(mCreateScale, mCreateRotation, _vec3(mCreatePos.x + i * mMultiCreateGap, mCreatePos.y, mCreatePos.z + j * mMultiCreateGap)));
				KObject* newObject = KObject::Create(info);
				mObjectList.emplace_back(newObject);
			}
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

}

//void CreativeMode::ReloadWorld(IWorldController* worldController)
//{
//	worldController->ClearObjectList();
//	worldController->SetUpObjectList(mObjectList);
//}

void CreativeMode::MappingObjectToObjectUI(const _int objectIndex)
{
	if (-1 == objectIndex)
	{
		KObject::Info info;
		mEditScale = info.Transform.Scale;
		mEditRotation = info.Transform.Rotation;
		mEditPosition = info.Transform.Position;

		mEditObjectType = info.Objecttype;
	}
	else
	{
		const KObject::Info& info = mObjectList[objectIndex]->GetInfo();
		mEditScale = info.Transform.Scale;
		mEditRotation = info.Transform.Rotation;
		mEditPosition = info.Transform.Position;

		mEditObjectType = info.Objecttype;
	}
}

void CreativeMode::MappingEditUIToEditObject(const _int objectIndex)
{
	if (0 > objectIndex)
		return;
	KObject::Info info(mEditObjectType, KEngine::Transform(mEditScale, mEditRotation, mEditPosition));
	mObjectList[objectIndex]->SetInfo(info);
}

void CreativeMode::LogicObject()
{
	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
		return;

	KeyManager* keyManager = KeyManager::GetInstance();

	_int selectedIndex;
	_vec3 hitWorldPos;
	const _bool isPicking = PickingObject(selectedIndex, hitWorldPos);

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (isPicking && keyManager->KeyPresseing(KeyManager::KEY_LSHIFT))
		{
			mCreatePos = hitWorldPos;
			CreateObject();
		}
		else
		{
			mSelectedObjectListIndex = selectedIndex;
			MappingObjectToObjectUI(mSelectedObjectListIndex);
		}
	}

	if (-1 != mSelectedObjectListIndex && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		if (isPicking && (selectedIndex != mSelectedObjectListIndex))
		{
			mEditPosition = hitWorldPos;
		}
	}
}

void CreativeMode::CreateObject()
{
	KObject::Info info(mCreateObjectType, KEngine::Transform(mCreateScale, mCreateRotation, mCreatePos));
	KObject* newObject = KObject::Create(info);
	mObjectList.emplace_back(newObject);
}

void CreativeMode::ClearObjectList()
{
	for (KObject*& object : mObjectList)
		SafeRelease(object);
	mObjectList.clear();
}

void CreativeMode::UpdateNaviMeshDataUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("NaviMeshData", ImVec2(0, 150), true, 0);

	ImGui::Text("[NaviMeshData]");
	ImGui::Text("Mode: ");

	ImGui::SameLine();
	if (ImGui::RadioButton("Create##NaviMeshCreate", mIsNaviMeshCreate))
		mIsNaviMeshCreate = true;

	ImGui::SameLine();
	if (ImGui::RadioButton("Edit##NaviMeshEdit", !mIsNaviMeshCreate))
		mIsNaviMeshCreate = false;

	ImGui::NewLine();
	ImGui::Text("Selected Point: ");

	ImGui::Text("Index:");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(30.f);

	const _int tempIndex = mSelectedNaviPointIndex;
	ImGui::InputInt("##SelectedNaviPointIndex", &mSelectedNaviPointIndex, 0, 0);
	if (mIsNaviMeshCreate)
		mSelectedNaviPointIndex = tempIndex;

	ImGui::SameLine();
	ImGui::Text("Pos:");

	ImGui::SameLine();
	ImGui::SetNextItemWidth(180.f);
	ImGui::InputFloat3("##SelectedNaviPointPos", mSelectedNaviPointPosition, "%.3f", ImGuiInputTextFlags_ReadOnly);


	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void CreativeMode::MappingNaviMeshDataToNaviMeshUI()
{
	mIsNaviMeshCreate = mNaviMeshData->IsCreateMode();
	mSelectedNaviPointIndex = mNaviMeshData->GetSelectedNaviPointIndex();
	mSelectedNaviPointPosition = mNaviMeshData->GetSelectedNaviPointPosition();
	mSelectedNaviCellIndex = mNaviMeshData->GetSelectedNaviCellIndex();
}

void CreativeMode::MappingNaviMeshUIToNaviMeshData()
{
	mNaviMeshData->SetCreateMode(mIsNaviMeshCreate);
	mNaviMeshData->SetSelectedNaviPointIndex(mSelectedNaviPointIndex);
	//mNaviMeshData->SetSelectedNaviPointPosition(mSelectedNaviPointPosition);
	mNaviMeshData->SetSelectedNaviCellIndex(mSelectedNaviCellIndex);
}
void CreativeMode::LogicNaviMeshCreateMode()
{
	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
		return;

	KeyManager* keyManager = KeyManager::GetInstance();

	if (keyManager->KeyDown(KeyManager::KEY_LSHIFT))
		mNaviMeshData->PopNaviPoint();

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		_bool isPicking = false;
		POINT mouse;
		::GetCursorPos(&mouse);
		::ScreenToClient(g_hWnd, &mouse);

		PipeLine::RayCast worldRayCast = PipeLine::GetInstance()->ConvertScreenPosToWorldRayCast(_vec2((_float)mouse.x, (_float)mouse.y));
		_int hitPointIndex = -1;
		isPicking = mNaviMeshData->CheckHitPoint(worldRayCast.RayPos, worldRayCast.RayDir, hitPointIndex);

		if (false == isPicking)
		{
			//	메시를 찍었을 경우
			_int selectedIndex;
			_vec3 hitWorldPos;
			isPicking = PickingObject(selectedIndex, hitWorldPos);
			if (isPicking)
			{
				NaviMeshData::PointStack newPointStack;
				newPointStack.Position = hitWorldPos;
				mNaviMeshData->PushNaviPoint(newPointStack, false);
			}
		}
		else
		{
			//	네비 메시의 정점을 찍었을 경우
			NaviMeshData::PointStack newPointStack;
			newPointStack.Index = hitPointIndex;
			mNaviMeshData->PushNaviPoint(newPointStack, true);
		}

	}
}

void CreativeMode::LogicNaviMeshEditMode()
{
	if (ImGui::IsAnyWindowHovered() || ImGui::IsAnyItemHovered())
		return;

	KeyManager* keyManager = KeyManager::GetInstance();

	POINT mouse;
	::GetCursorPos(&mouse);
	::ScreenToClient(g_hWnd, &mouse);

	PipeLine::RayCast worldRayCast = PipeLine::GetInstance()->ConvertScreenPosToWorldRayCast(_vec2((_float)mouse.x, (_float)mouse.y));

	if (-1 != mSelectedNaviPointIndex)
	{
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			int selectedIndex = -1;
			_vec3 hitWorldPos;
			const _bool isObjectPicking = PickingObject(selectedIndex, hitWorldPos);
			if (isObjectPicking)
			{
				mSelectedNaviPointPosition = hitWorldPos;

				mNaviMeshData->SetSelectedNaviPointPosition(mSelectedNaviPointPosition);
				return;
			}
		}
	}
	else if (-1 != mSelectedNaviCellIndex)
	{
		if (keyManager->KeyDown(KeyManager::KEY_LSHIFT))
		{
			mNaviMeshData->DeleteCell(mSelectedNaviCellIndex);
			mSelectedNaviCellIndex = -1;
			return;
		}
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		_int	hitIndex = -1;
		_bool	isPicking = mNaviMeshData->CheckHitPoint(worldRayCast.RayPos, worldRayCast.RayDir, hitIndex);

		mSelectedNaviPointIndex = -1;
		mSelectedNaviCellIndex = -1;

		if (isPicking)
		{
			mSelectedNaviPointIndex = hitIndex;

			return;
		}

		isPicking = mNaviMeshData->CheckHitCell(worldRayCast.RayPos, worldRayCast.RayDir, hitIndex);
		
		if (isPicking)
		{
			mSelectedNaviCellIndex = hitIndex;
			return;
		}

	}
}

_bool CreativeMode::Initialize(LPDIRECT3DDEVICE9 graphicDevice)
{
	_matrix scaleMatrix = *D3DXMatrixScaling(&scaleMatrix, 0.01f, 0.01f, 0.01f);

	mPlayerMesh = DynamicMesh_Object::Create(graphicDevice, L"..\\Resources\\Player\\", L"Player.X", scaleMatrix);
	if (nullptr == mPlayerMesh)
		return false;

	mBotMesh = DynamicMesh_Object::Create(graphicDevice, L"..\\Resources\\Y_Bot\\", L"Y_Bot.X", scaleMatrix);
	if (nullptr == mBotMesh)
		return false;

	mBlockMesh = StaticMesh_Object::Create(graphicDevice, L"..\\Resources\\Block\\", L"Block.X", scaleMatrix);
	if (nullptr == mBlockMesh)
		return false;

	mInstancingShader = Shader::Create(graphicDevice, L"..\\Shader\\HardwareInstancing.fx");
	if (nullptr == mInstancingShader)
		return false;

	mMeshShader = Shader::Create(graphicDevice, L"..\\Shader\\MeshRender.fx");
	if (nullptr == mMeshShader)
		return false;

	mSelectedMeshShader = Shader::Create(graphicDevice, L"..\\Shader\\SelectedMeshRender.fx");
	if (nullptr == mSelectedMeshShader)
		return false;

	D3DVERTEXELEMENT9 vertexElem[] =
	{
		{ 0, 0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,	D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,   0 },
		{ 0, 24,	D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 0 },
		{ 1, 0,		D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 1 },
		{ 1, 16,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 2 },
		{ 1, 32,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 3 },
		{ 1, 48,	D3DDECLTYPE_FLOAT4,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD, 4 },
		D3DDECL_END()
	};

	//	For. Hardware Instancing
	HRESULT hr = 0;

	hr = graphicDevice->CreateVertexBuffer(mBlockRenderBatchSize * sizeof(_matrix), 0, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, &mVertexBuffer, nullptr);
	if (FAILED(hr))
		return false;


	hr = graphicDevice->CreateVertexDeclaration(vertexElem, &mVertexDeclaration);
	if (FAILED(hr))
		return false;

	mIsNaviMeshCreate = true;

	mNaviMeshData = NaviMeshData::Create(graphicDevice);
	if (nullptr == mNaviMeshData)
		return false;

	return true;
}

void CreativeMode::InitSampleData()
{
	mObjectList.emplace_back(KObject::Create(KObject::Info(Game::ObjectType::Player,
		KEngine::Transform(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), _vec3(0.f, 0.f, 0.f)))));

	constexpr _float gap = 0.5f;
	for (_int i = -4; i < 0; ++i)
		for (_int j = 0; j < 4; ++j)
			mObjectList.emplace_back(KObject::Create(KObject::Info(Game::ObjectType::Bot,
				KEngine::Transform(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), _vec3(i * gap, 0.f, j * gap)))));

	for (_int i = 4; i > 0; --i)
		for (_int j = 0; j < 4; ++j)
			mObjectList.emplace_back(KObject::Create(KObject::Info(Game::ObjectType::Block,
				KEngine::Transform(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), _vec3(i * gap, 0.f, j * gap)))));
}

void CreativeMode::RenderSkinnedMesh(Shader* shader, KObject *& object, DynamicMesh_Object * dynamicMesh)
{
	shader->SetValue("gMatWorld", &object->GetWorldMatrix(), sizeof(_matrix));
	for (_int i = 0; i < dynamicMesh->GetMeshContinerSize(); ++i)
	{
		D3DXMESHCONTAINER_DERIVED* meshContainer = dynamicMesh->GetMeshContainer(i);
		for (_int j = 0; j < (_int)meshContainer->NumMaterials; ++j)
		{
			shader->SetTexture("gDiffuseTexture", meshContainer->pMeshTexture[j]);
			shader->CommitChanges();
			meshContainer->pSoftwareMesh->DrawSubset(j);
		}
	}
}

void CreativeMode::RenderInstanceMesh(LPDIRECT3DDEVICE9 graphicDevice, vector<KObject*>& objectList, StaticMesh_Object * staticMesh)
{
	PipeLine* pipeLine = PipeLine::GetInstance();
	const _matrix matVP = pipeLine->GetTransform(D3DTS_VIEW) * pipeLine->GetTransform(D3DTS_PROJECTION);
	mInstancingShader->SetValue("gMatVP", &matVP, sizeof(_matrix));
	mInstancingShader->SetTexture("gDiffuseTexture", staticMesh->GetTexutre(0));

	//mInstancingShader->BeginShader(nullptr);
	//mInstancingShader->BeginPass(0);

	_int numBlock = 0;
	_matrix*	worldMatrixList = nullptr;

	mVertexBuffer->Lock(0, 0, (void**)&worldMatrixList, D3DLOCK_DISCARD);

	for (_int i = 0; i < objectList.size(); ++i)
	{
		//memcpy(&worldMatrixList[i], &objectList[i]->GetWorldMatrix(), sizeof(_matrix));
		_int index = i % mBlockRenderBatchSize;
		worldMatrixList[index] = objectList[i]->GetWorldMatrix();

		++numBlock;

		if (mBlockRenderBatchSize == numBlock)
		{
			mVertexBuffer->Unlock();

			RenderHardwareInstancing(graphicDevice, staticMesh, numBlock, mInstancingShader);
			numBlock = 0;

			mVertexBuffer->Lock(0, 0, (void**)&worldMatrixList, D3DLOCK_DISCARD);
		}
	}
	mVertexBuffer->Unlock();

	if (0 < numBlock)
	{
		RenderHardwareInstancing(graphicDevice, staticMesh, numBlock, mInstancingShader);
	}

	worldMatrixList = nullptr;

	//mInstancingShader->EndPass();
	//mInstancingShader->EndShader();
}

void CreativeMode::RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMesh_Object * staticMesh, _int numBlock, Shader * shader)
{
	graphicDevice->SetVertexDeclaration(mVertexDeclaration);
	graphicDevice->SetStreamSource(0, staticMesh->GetVertexBuffer(), 0, staticMesh->GetVertexSize());
	graphicDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numBlock);

	graphicDevice->SetStreamSource(1, mVertexBuffer, 0, sizeof(_matrix));
	graphicDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
	graphicDevice->SetIndices(staticMesh->GetIndexBuffer());

	shader->BeginShader(nullptr);
	shader->BeginPass(0);

	graphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, staticMesh->GetVertexNum(), 0, staticMesh->GetFacesNum());

	shader->EndPass();
	shader->EndShader();

	graphicDevice->SetStreamSourceFreq(0, 1);
	graphicDevice->SetStreamSourceFreq(1, 1);

}

CreativeMode * CreativeMode::Create(LPDIRECT3DDEVICE9 graphicDevice)
{
	CreativeMode*	pInstance = new CreativeMode();

	if (false == pInstance->Initialize(graphicDevice))
	{
		MSG_BOX("CreativeMode Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CreativeMode::Free()
{
	SafeRelease(mNaviMeshData);

	SafeRelease(mVertexBuffer);

	SafeRelease(mVertexDeclaration);

	SafeRelease(mSelectedMeshShader);
	SafeRelease(mMeshShader);
	SafeRelease(mInstancingShader);

	SafeRelease(mPlayerMesh);
	SafeRelease(mBotMesh);
	SafeRelease(mBlockMesh);

	ClearObjectList();

	Mode::Free();
}
