#include "stdafx.h"
#include "World_Object.h"

//	Core
#include "PipeLine.h"

//	Component
#include "ComponentManager_Object.h"
#include "Transform_Object.h"
#include "DynamicMesh_Object.h"
#include "Shader.h"

//	GameObject
#include "GameObject.h"
#include "Bot.h"

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
	for (GameObject* gameObject : mGameObjectList)
		gameObject->Update(timeDelta);

	for (GameObject* gameObject : mGameObjectList)
		gameObject->LateUpdate(timeDelta);
}

void World_Object::Render()
{
	//LPDIRECT3DDEVICE9 graphicDevice = World::GetGraphicDev();
	//graphicDevice->SetTransform(D3DTS_VIEW, &mPipeLine->GetTransform(D3DTS_VIEW));
	//graphicDevice->SetTransform(D3DTS_PROJECTION, &mPipeLine->GetTransform(D3DTS_PROJECTION));
	
	for (GameObject* gameObject : mGameObjectList)
		gameObject->Render();
}

_bool World_Object::Clear()
{
	for (GameObject* gameObject : mGameObjectList)
		SafeRelease(gameObject);

	mGameObjectList.clear();

	return true;
}

_bool World_Object::Initialize()
{
	if (false == ReadyComponent())
		return false;

	if (false == ReadyGameObject())
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

	//	Ready: Component
	mComponentManager->AddPrototype("Transform", Transform_Object::Create());
	mComponentManager->AddPrototype("DynamicMesh_Bot", DynamicMesh_Object::Create(World::GetGraphicDevice(), L"..\\Resources\\Y_Bot\\", L"Y_Bot.X"));
	mComponentManager->AddPrototype("Shader_HardwareSkinning", Shader::Create(World::GetGraphicDevice(), L"..\\Shader\\HardwareSkinning.fx"));

	return true;
}

_bool World_Object::ReadyGameObject()
{
	for(_int i = -5; i < 5; ++i)
		for(_int j = -5; j < 5; ++j)
			SetUpBot(_vec3(i * 0.35f, 0.f, j * 0.35f));
	//SetUpBot(_vec3(0.f, 0.f, 0.f));
	return true;
}

_bool World_Object::SetUpBot(const _vec3 position)
{
	GameObject* gameObject = nullptr;
	//gameObject = Bot::Create(Bot::Data(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), position));
	//gameObject = Bot::Create(Bot::Data(_vec3(0.1f, 0.1f, 0.1f), _vec3(0.f, 0.f, 0.f), position));
	gameObject = Bot::Create(Bot::Data(_vec3(1.f, 1.f, 1.f), _vec3(0.f, 0.f, 0.f), position));
	
	if (nullptr == gameObject)
		return false;

	gameObject->SetUp(this);
	mGameObjectList.emplace_back(gameObject);
	return true;
}

void World_Object::Free()
{
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
