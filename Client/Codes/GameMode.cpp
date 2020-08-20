#include "stdafx.h"
#include "GameMode.h"

#include "KObject.h"

GameMode::GameMode()
{
}

void GameMode::Active(IWorldController* worldController)
{
}

void GameMode::InActive(IWorldController* worldController)
{
}

void GameMode::Update(IWorldController* worldController)
{
}

void GameMode::Render(LPDIRECT3DDEVICE9 graphicDevice)
{
}

_bool GameMode::Initialize(LPDIRECT3DDEVICE9 graphicDevice)
{
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
	ClearObjectList();

	Mode::Free();
}


