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
#include "DynamicMesh_Object.h"
#include "StaticMesh_Object.h"
#include "WorldController.h"
#include "PipeLine.h"
#include "Shader.h"


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
	/*ReloadWorld(worldController);*/

	mbDisplayObjectFilter.fill(true);
	mDisplayObjectList.reserve(mObjectList.size());
	UpdateDisplayList();
}

void CreativeMode::InActive(IWorldController* worldController)
{
	ClearDisplayObjectList();
	ClearObjectList();
}

void CreativeMode::Update(IWorldController* worldController)
{
	UpdateDisplayObjectListUI(worldController);
	UpdateEditorUI();
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		PickingObject();
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

	//graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

void CreativeMode::PickingObject()
{
	POINT mouse;
	::GetCursorPos(&mouse);
	::ScreenToClient(g_hWnd, &mouse);

	PipeLine::RayCast rayCast = PipeLine::GetInstance()->ConvertScreenPosToRayCast(_vec2(mouse.x, mouse.y));

	_float nearestDis = 3.402823466e+38f;
	for (_int i = 0; i < mObjectList.size(); ++i)
	{
		KObject*& object = mObjectList[i];
		const _matrix matInverseWorld = object->GetWorldInverseMatrix();

		_vec3 localRayPos, localRayDir;
		D3DXVec3TransformCoord(&localRayPos, &rayCast.RayPos, &matInverseWorld);
		D3DXVec3TransformNormal(&localRayDir, &rayCast.RayDir, &matInverseWorld);

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
						mSelectedObjectListIndex = i;
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
						mSelectedObjectListIndex = i;
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
					mSelectedObjectListIndex = i;
					nearestDis = hitDis;
				}
			}
		}
		break;

		}
	}
}

void CreativeMode::UpdateDisplayObjectListUI(IWorldController* worldController)
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

		//ReloadWorld(worldController);
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

	ImGui::Text("Scale:");
	ImGui::SameLine();
	ImGui::InputFloat3("", mCreateScale);

	ImGui::Text("Rotation:");
	ImGui::SameLine();
	ImGui::InputFloat3("", mCreateRotation);

	ImGui::Text("Position:");
	ImGui::SameLine();
	ImGui::InputFloat3("", mCreatePos);

	ImGui::Text("ObjectType:");

	ImGui::SameLine();
	if (ImGui::RadioButton("Player", Game::Player == mCurSelectedObjectType))
		mCurSelectedObjectType = Game::Player;

	ImGui::SameLine();
	if (ImGui::RadioButton("Bot", Game::Bot == mCurSelectedObjectType))
		mCurSelectedObjectType = Game::Bot;

	ImGui::SameLine();
	if (ImGui::RadioButton("Block", Game::Block == mCurSelectedObjectType))
		mCurSelectedObjectType = Game::Block;

	ImGui::NewLine();
	ImGui::SameLine(ImGui::GetWindowWidth() - 40);
	if (ImGui::Button("Add"))
	{

	}

	ImGui::EndChild();
	ImGui::PopStyleVar();

}

void CreativeMode::ReloadWorld(IWorldController* worldController)
{
	worldController->ClearObjectList();
	worldController->SetUpObjectList(mObjectList);
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

	hr = graphicDevice->CreateVertexBuffer(mBlockRenderBatchSize * sizeof(_matrix), 0, 0, D3DPOOL_MANAGED, &mVertexBuffer, nullptr);
	if (FAILED(hr))
		return false;

	hr = graphicDevice->CreateVertexDeclaration(vertexElem, &mVertexDeclaration);
	if (FAILED(hr))
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
		for (_int j = 0; j < meshContainer->NumMaterials; ++j)
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

	mInstancingShader->BeginShader(nullptr);
	mInstancingShader->BeginPass(0);

	_int numBlock = 0;
	_matrix*	worldMatrixList = nullptr;

	mVertexBuffer->Lock(0, 0, (void**)&worldMatrixList, 0);
	for (_int i = 0; i < objectList.size(); ++i)
	{
		memcpy(worldMatrixList[i], objectList[i]->GetWorldMatrix(), sizeof(_matrix));

		++numBlock;

		if (mBlockRenderBatchSize == numBlock)
		{
			mVertexBuffer->Unlock();
			RenderHardwareInstancing(graphicDevice, staticMesh, numBlock, mInstancingShader);
			numBlock = 0;
			mVertexBuffer->Lock(0, 0, (void**)&worldMatrixList, 0);
		}
	}
	mVertexBuffer->Unlock();

	if (0 < numBlock)
		RenderHardwareInstancing(graphicDevice, staticMesh, numBlock, mInstancingShader);

	worldMatrixList = nullptr;

	mInstancingShader->EndPass();
	mInstancingShader->EndShader();
}

void CreativeMode::RenderHardwareInstancing(LPDIRECT3DDEVICE9 graphicDevice, StaticMesh_Object * staticMesh, _int numBlock, Shader * shader)
{
	LPDIRECT3DVERTEXBUFFER9 blockVB = nullptr;
	staticMesh->GetVertexBuffer(blockVB);

	LPDIRECT3DINDEXBUFFER9	blockIB = nullptr;
	staticMesh->GetIndexBuffer(blockIB);

	const _ulong blockVertexSize = staticMesh->GetVertexSize();

	graphicDevice->SetVertexDeclaration(mVertexDeclaration);
	graphicDevice->SetStreamSource(0, blockVB, 0, blockVertexSize);
	graphicDevice->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numBlock);

	graphicDevice->SetStreamSource(1, mVertexBuffer, 0, sizeof(_matrix));
	graphicDevice->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
	graphicDevice->SetIndices(blockIB);

	graphicDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, staticMesh->GetVertexNum(), 0, staticMesh->GetFacesNum());

	SafeRelease(blockIB);
	SafeRelease(blockVB);

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
	SafeRelease(mVertexBuffer);
	SafeRelease(mVertexDeclaration);

	SafeRelease(mSelectedMeshShader);
	SafeRelease(mMeshShader);
	SafeRelease(mInstancingShader);

	SafeRelease(mPlayerMesh);
	SafeRelease(mBotMesh);
	SafeRelease(mBlockMesh);

	ClearDisplayObjectList();
	ClearObjectList();

	Mode::Free();
}
