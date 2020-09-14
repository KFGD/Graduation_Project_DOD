#include "stdafx.h"
#include "World_Object.h"

//	Core
#include "PipeLine.h"

#include "KObject.h"

//	Component
#include "ComponentManager_Object.h"
#include "Transform_Object.h"
#include "StaticMeshRenderer_Object.h"
#include "DynamicMeshRenderer_Object.h"
#include "NaviMesh_Object.h"
#include "Shader.h"

#include "KObject.h"
#include "CameraController.h"

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
	EASY_FUNCTION(profiler::colors::Red);
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

	RenderBlock();

	//mNaviMesh->Render(GetGraphicDevice());
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

	for (GameObject* object : mPlayerList)
		object->SetUp(this);

	for (GameObject* object : mBotList)
		object->SetUp(this);

	for (GameObject* object : mBlockList)
		object->SetUp(this);

	if(!mPlayerList.empty())
		CameraController::GetInstance()->SetCameraTarget(mPlayerList.front());

	return true;
}

_bool World_Object::SetUpNaviMesh(const NaviMeshData * naviMeshData)
{
	mNaviMesh->SetNaviMeshData(*naviMeshData);
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

	hr = GetGraphicDevice()->CreateVertexBuffer(mBlockRenderBatchSize * sizeof(_matrix), 0, 0, D3DPOOL_MANAGED, &mVertexBuffer, nullptr);
	if (FAILED(hr))
		return false;

	hr = GetGraphicDevice()->CreateVertexDeclaration(vertexElem, &mVertexDeclaration);
	if (FAILED(hr))
		return false;

	//	For. NaviMesh
	mNaviMesh = NaviMesh_Object::Create(GetGraphicDevice());
	if (nullptr == mNaviMesh)
		return false;

	return true;
}

_bool World_Object::ReadyComponent()
{
	if (nullptr == mComponentManager)
		return false;

	_matrix scaleMatrix = *D3DXMatrixScaling(&scaleMatrix, 0.01f, 0.01f, 0.01f);

	//	Ready: Component
	mComponentManager->AddPrototype("Transform", Transform_Object::Create());
	
	mComponentManager->AddPrototype("DynamicMesh_Player", DynamicMeshRenderer_Object::Create(World::GetGraphicDevice(), L"..\\Resources\\Player\\", L"Player.X", scaleMatrix));
	mComponentManager->AddPrototype("DynamicMesh_Bot", DynamicMeshRenderer_Object::Create(World::GetGraphicDevice(), L"..\\Resources\\Y_Bot\\", L"Y_Bot.X", scaleMatrix));
	mComponentManager->AddPrototype("Shader_HardwareSkinning", Shader::Create(World::GetGraphicDevice(), L"..\\Shader\\HardwareSkinning.fx"));

	mComponentManager->AddPrototype("StaticMesh_GrayBlock", StaticMeshRenderer_Object::Create(World::GetGraphicDevice(), L"..\\Resources\\Block\\", L"Block.X", scaleMatrix));
	mComponentManager->AddPrototype("Shader_HardwareInstancing", Shader::Create(World::GetGraphicDevice(), L"..\\Shader\\HardwareInstancing.fx"));

	return true;
}

void World_Object::RenderBlock()
{
	if (0 == mBlockList.size())
		return;



	Shader*	shader = mBlockList[0]->GetSahder();
	StaticMeshRenderer_Object*	staticMesh = mBlockList[0]->GetStaticMesh();

	const _matrix matVP = mPipeLine->GetTransform(D3DTS_VIEW) * mPipeLine->GetTransform(D3DTS_PROJECTION);

	shader->SetValue("gMatVP", &matVP, sizeof(_matrix));
	shader->SetTexture("gDiffuseTexture", staticMesh->GetTexutre(0));

	shader->BeginShader(nullptr);
	shader->BeginPass(0);

	_int numBlock = 0;
	_matrix*	worldMatrixList = nullptr;

	mVertexBuffer->Lock(0, 0, (void**)&worldMatrixList, 0);
	for (_int i = 0; i < mBlockList.size(); ++i)
	{
		_int index = i % mBlockRenderBatchSize;
		worldMatrixList[index] = mBlockList[i]->GetTransform()->GetWorldMatrix();
		//memcpy(worldMatrixList[i], mBlockList[i]->GetTransform()->GetWorldMatrix(), sizeof(_matrix));

		++numBlock;

		if (mBlockRenderBatchSize == numBlock)
		{
			mVertexBuffer->Unlock();
			RenderHardwareInstancing(staticMesh, numBlock);
			numBlock = 0;
			mVertexBuffer->Lock(0, 0, (void**)&worldMatrixList, 0);
		}
	}
	mVertexBuffer->Unlock();

	if (0 < numBlock)
		RenderHardwareInstancing(staticMesh, numBlock);

	worldMatrixList = nullptr;

	shader->EndPass();
	shader->EndShader();
}

void World_Object::RenderHardwareInstancing(StaticMeshRenderer_Object* staticMesh, _int numBlock)
{
	LPDIRECT3DVERTEXBUFFER9 blockVB = nullptr;
	staticMesh->GetVertexBuffer(blockVB);

	LPDIRECT3DINDEXBUFFER9	blockIB = nullptr;
	staticMesh->GetIndexBuffer(blockIB);

	const _ulong blockVertexSize = staticMesh->GetVertexSize();

	GetGraphicDevice()->SetVertexDeclaration(mVertexDeclaration);
	GetGraphicDevice()->SetStreamSource(0, blockVB, 0, blockVertexSize);
	GetGraphicDevice()->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | numBlock);

	GetGraphicDevice()->SetStreamSource(1, mVertexBuffer, 0, sizeof(_matrix));
	GetGraphicDevice()->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);
	GetGraphicDevice()->SetIndices(blockIB);
	
	GetGraphicDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, staticMesh->GetVertexNum(), 0, staticMesh->GetFacesNum());

	SafeRelease(blockIB);
	SafeRelease(blockVB);

	GetGraphicDevice()->SetStreamSourceFreq(0, 1);
	GetGraphicDevice()->SetStreamSourceFreq(1, 1);
}

void World_Object::Free()
{
	SafeRelease(mNaviMesh);

	SafeRelease(mVertexBuffer);
	SafeRelease(mVertexDeclaration);

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
