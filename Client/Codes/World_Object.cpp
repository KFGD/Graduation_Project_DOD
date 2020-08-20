#include "stdafx.h"
#include "World_Object.h"

//	Core
#include "PipeLine.h"

#include "KObject.h"

//	Component
#include "ComponentManager_Object.h"
#include "Transform_Object.h"
#include "StaticMesh_Object.h"
#include "DynamicMesh_Object.h"
#include "Shader.h"

#include "KObject.h"

//	GameObject
#include "GameObject.h"
#include "Player.h"
#include "Bot.h"
#include "BlockObj.h"

World_Object::World_Object(const LPDIRECT3DDEVICE9 graphicDev)
	: World(graphicDev)
	, mPipeLine(PipeLine::GetInstance())
	, mComponentManager(ComponentManager_Object::GetInstance())
{
	SafeAddRef(mPipeLine);
	SafeAddRef(mComponentManager);
}


void World_Object::Update(const _double timeDelta)
{
	//	Update
	for (GameObject* gameObject : mPlayerList)
		gameObject->Update(timeDelta);
	for (GameObject* gameObject : mBotList)
		gameObject->Update(timeDelta);
	for (GameObject* gameObject : mBlockList)
		gameObject->Update(timeDelta);

	//	LateUpdate
	for (GameObject* gameObject : mPlayerList)
		gameObject->LateUpdate(timeDelta);
	for (GameObject* gameObject : mBotList)
		gameObject->LateUpdate(timeDelta);
	for (GameObject* gameObject : mBlockList)
		gameObject->LateUpdate(timeDelta);
}

void World_Object::Render()
{
	for (GameObject* gameObject : mPlayerList)
		gameObject->Render();
	for (GameObject* gameObject : mBotList)
		gameObject->Render();
	for (GameObject* gameObject : mBlockList)
		gameObject->Render();
}

_bool World_Object::SetUpObjectList(const vector<KObject*>& objectList)
{
	for (KObject* object : objectList)
	{
		KObject::Info info = object->GetInfo();
		switch (info.Objecttype)
		{
		case Game::Player:
		{
			typedef	Player::Data	Data;
			mPlayerList.emplace_back(Player::Create(Data(info.Transform.Scale, info.Transform.Rotation, info.Transform.Position)));
		}
			break;

		case Game::Bot:
		{
			typedef	Bot::Data	Data;
			mBotList.emplace_back(Bot::Create(Data(info.Transform.Scale, info.Transform.Rotation, info.Transform.Position)));
		}
			break;

		case Game::Block:
		{
			typedef	BlockObj::Data	Data;
			mBlockList.emplace_back(BlockObj::Create(Data(info.Transform.Scale, info.Transform.Rotation, info.Transform.Position)));
		}
			break;

		default:
			break;
		}
	}

	return true;
}

_bool World_Object::Clear()
{
	for (GameObject* gameObject : mPlayerList)
		SafeRelease(gameObject);
	mPlayerList.clear();

	for (GameObject* gameObject : mBotList)
		SafeRelease(gameObject);
	mBotList.clear();

	for (GameObject* gameObject : mBlockList)
		SafeRelease(gameObject);
	mBlockList.clear();

	return true;
}

_bool World_Object::Initialize()
{
	if (false == ReadyComponent())
		return false;
	
	_matrix viewMatrix = *D3DXMatrixLookAtLH(&viewMatrix, &_vec3(0.f, 3.f, 3.f), &_vec3(0.f, 0.f, 0.f), &_vec3(0.f, 1.f, 0.f));
	_matrix projMatrix = *D3DXMatrixPerspectiveFovLH(&projMatrix, D3DXToRadian(60.f), (_float)WINCX / (_float)WINCY, 0.01f, 1000.f);

	mPipeLine->SetTransform(D3DTS_VIEW, viewMatrix);
	mPipeLine->SetTransform(D3DTS_PROJECTION, projMatrix);

	return true;
}

_bool World_Object::ReadyComponent()
{
	if (nullptr == mComponentManager)
		return false;

	_matrix scaleMatrix = *D3DXMatrixScaling(&scaleMatrix, 0.01f, 0.01f, 0.01f);

	//	Ready: Component
	mComponentManager->AddPrototype("Transform", Transform_Object::Create());
	
	mComponentManager->AddPrototype("DynamicMesh_Bot", DynamicMesh_Object::Create(World::GetGraphicDevice(), L"..\\Resources\\Y_Bot\\", L"Y_Bot.X"));
	mComponentManager->AddPrototype("Shader_HardwareSkinning", Shader::Create(World::GetGraphicDevice(), L"..\\Shader\\HardwareSkinning.fx"));

	mComponentManager->AddPrototype("StaticMesh_GrayBlock", StaticMesh_Object::Create(World::GetGraphicDevice(), L"..\\Resources\\Block\\", L"Block.X", scaleMatrix));
	mComponentManager->AddPrototype("Shader_HardwareInstancing", Shader::Create(World::GetGraphicDevice(), L"..\\Shader\\HardwareInstancing.fx"));

	return true;
}

void World_Object::Free()
{
	Clear();

	SafeRelease(mPipeLine);
	SafeRelease(mComponentManager);

	World::Free();
}

World_Object * World_Object::Create(const LPDIRECT3DDEVICE9 graphicDev)
{
	World_Object*	pInstance = new World_Object(graphicDev);
	
	if (false == pInstance->Initialize())
	{
		MSG_BOX("World_Object Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}
