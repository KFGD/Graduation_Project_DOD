#include "stdafx.h"
#include "GameMode.h"

#include "KObject.h"
#include "World_Object.h"

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

void GameMode::Update(const _double timeDelta)
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

	mCurWorldType = Game::Object_Oriented;

	return true;
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


