#include "stdafx.h"
#include "Bot.h"

#include "Transform_Object.h"
#include "DynamicMesh_Object.h"

#include "World_Object.h"
#include "PipeLine.h"
#include "Shader.h"
#include "NaviMesh_Object.h"

Bot::Bot()
{
}

void Bot::SetUp(World_Object * world)
{
	GameObject::SetUp(world);

	NaviMesh_Object* naviMesh = world->GetNaviMeshObject();
	mCellIndex = naviMesh->FindCellIndex(mTransform->GetPosition());
}

void Bot::Update(const _double timeDelta)
{
	mTransform->CalculateWorldMatrix();
}

void Bot::LateUpdate(const _double timeDelta)
{
	mTimeDelta = timeDelta;

	World_Object* world = GetWorld();
	NaviMesh_Object* naviMesh = world->GetNaviMeshObject();

	_int nextCellIndex = 0;
	_vec3 fixPosition;
	if (naviMesh->Move(mCellIndex, mTransform->GetPosition(), nextCellIndex, fixPosition))
	{
		mCellIndex = nextCellIndex;
		mTransform->SetPosition(fixPosition);
	}
}

void Bot::Render()
{
	return;

	mDynamicMesh->PlayAnimation(mTimeDelta);

	PipeLine* pipeLine = PipeLine::GetInstance();
	const _matrix matVP = pipeLine->GetTransform(D3DTS_VIEW) * pipeLine->GetTransform(D3DTS_PROJECTION);
	mShader->Get_EffectHandle()->SetMatrix("gMatWorld", &mTransform->GetWorldMatrix());
	mShader->Get_EffectHandle()->SetMatrix("gMatVP", &matVP);
	
	for (_int i = 0; i < mDynamicMesh->GetMeshContinerSize(); ++i)
		mDynamicMesh->Render(mShader, i);
}

_bool Bot::Initialize(const Bot::Data & data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("DynamicMesh_Bot", "DynamicMesh_Bot", (Component_Object**)&mDynamicMesh);
	GameObject::AddComponent("Shader_HardwareSkinning", "Shader_HardwareSkinning", (Component_Object**)&mShader);
	
	mDynamicMesh->SetUpAnimation(0);

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
		MSG_BOX("Failed To Create Bot Instance");
	}

	return pInstnace;
}
