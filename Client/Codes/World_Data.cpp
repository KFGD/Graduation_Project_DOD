#include "stdafx.h"
#include "World_Data.h"

#include "KObject.h"

#include "EntityManager.h"
#include "TransformSystem.h"
#include "StaticRendererSystem.h"
#include "Entity.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"

World_Data::World_Data(const LPDIRECT3DDEVICE9 graphicDevice)
	: World(graphicDevice)
	, mEntityManager(EntityManager::GetInstance())
	, mTransformSystem(TransformSystem::GetInstance())
	, mStaticRendererSystem(StaticRendererSystem::GetInstance())
{
	SafeAddRef(mEntityManager);
	SafeAddRef(mTransformSystem);
	SafeAddRef(mStaticRendererSystem);
}

void World_Data::Update(const _double timeDelta)
{
	EASY_FUNCTION(profiler::colors::Yellow);
	mTransformSystem->Update(timeDelta);
	mStaticRendererSystem->Update(timeDelta);

	mTransformSystem->LateUpdate(timeDelta);
	mStaticRendererSystem->LateUpdate(timeDelta);
}

void World_Data::Render()
{
	LPDIRECT3DDEVICE9 graphicDevice = World::GetGraphicDev();
	mStaticRendererSystem->Render(graphicDevice);
}

_bool World_Data::SetUpObjectList(const vector<KObject*>& objectList)
{
	const _size_t entitySize = objectList.size();

	_size_t blockNum = 0;
	for (KObject* object : objectList)
	{
		if (Game::Block == object->GetInfo().Objecttype)
			blockNum += 1;
	}

	mEntityManager->ReadySystem(entitySize);
	mTransformSystem->ReadySystem(entitySize);
	mStaticRendererSystem->ReadySystem(entitySize, blockNum);

	for (KObject* object : objectList)
	{
		Entity* entity = mEntityManager->CreateEntity();
		if (nullptr == entity)
			break;

		KObject::Info info = object->GetInfo();
		switch (info.Objecttype)
		{
		case Game::Player:
		{
		}
		break;

		case Game::Bot:
		{
		}
		break;

		case Game::Block:
		{
			const _uniqueId entityId = entity->GetUniqueId();
			mTransformSystem->AttachComponent(entityId, info.Transform.Scale, info.Transform.Rotation, info.Transform.Position);
			mStaticRendererSystem->AttachComponent(entityId, "GrayBlock");
		}
		break;

		default:
			break;
		}
	}

	return true;
}

_bool World_Data::SetUpNaviMesh(const NaviMeshData * naviMeshData)
{
	return true;
}

_bool World_Data::Clear()
{
	mEntityManager->ClearSystem();
	mTransformSystem->ClearSystem();
	mStaticRendererSystem->ClearSystem();

	return true;
}

_bool World_Data::Initialize()
{
	if (!ReadyResources())
		return false;

	return true;
}

_bool World_Data::ReadyResources()
{
	LPDIRECT3DDEVICE9 graphicDevice = World::GetGraphicDev();

	if (!mStaticRendererSystem->ReadyResources(graphicDevice))
		return false;

	_matrix scaleMatrix = *D3DXMatrixScaling(&scaleMatrix, 0.01f, 0.01f, 0.01f);

	StaticMesh* blockMesh = StaticMesh::Create(graphicDevice, L"..\\Resources\\Block\\", L"Block.X", scaleMatrix);
	if (nullptr == blockMesh)
		return false;

	mStaticRendererSystem->AddStaticMesh("GrayBlock", blockMesh);

	DynamicMesh* playerMesh = DynamicMesh::Create(graphicDevice, L"..\\Resources\\Player\\", L"Player.X", scaleMatrix);
	if (nullptr == playerMesh)
		return false;
	SafeRelease(playerMesh);

	return true;
}

void World_Data::Free()
{
	SafeRelease(mEntityManager);
	SafeRelease(mTransformSystem);
	SafeRelease(mStaticRendererSystem);

	World::Free();
}

World_Data * World_Data::Create(const LPDIRECT3DDEVICE9 graphicDevice)
{
	World_Data*	pInstance = new World_Data(graphicDevice);

	if (false == pInstance->Initialize())
	{
		MSG_BOX("World_Data Created Failed!");
		SafeRelease(pInstance);
	}

	return pInstance;
}
