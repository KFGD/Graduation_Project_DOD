#include "stdafx.h"
#include "Bot.h"

#include "Transform_Object.h"
#include "DynamicMesh_Object.h"

#include "World.h"

Bot::Bot()
{
}

void Bot::Update(const _double timeDelta)
{
	mTransform->CalculateWorldMatrix();
}

void Bot::LateUpdate(const _double timeDelta)
{
	mDynamicMesh->PlayerAnimation(timeDelta);
}

void Bot::Render()
{
	World* world = GetWorld();
	LPDIRECT3DDEVICE9 graphicDevice = world->GetGraphicDevice();

	const _matrix& worldMatrix = mTransform->GetWorldMatrix();
	graphicDevice->SetTransform(D3DTS_WORLD, &worldMatrix);

	for (_int i = 0; i < mDynamicMesh->GetMeshContinerSize(); ++i)
		mDynamicMesh->UpdateSkinnedMesh(i);

	mDynamicMesh->Render(graphicDevice);

}

_bool Bot::Initialize(const Bot::Data & data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("DynamicMesh_Player", "DynamicMesh_Player", (Component_Object**)&mDynamicMesh);

	return true;
}

void Bot::Free()
{
	GameObject::Free();
}

Bot * Bot::Create(const Bot::Data & data)
{
	Bot*	pInstnace = new Bot();
	
	if (false == pInstnace->Initialize(data))
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create Player Instance");
	}

	return pInstnace;
}
