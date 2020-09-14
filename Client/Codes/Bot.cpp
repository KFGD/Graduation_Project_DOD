#include "stdafx.h"
#include "Bot.h"

#include "BotIdle.h"
#include "BotRun.h"

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
	if (mCurState != mNextState)
	{
		if (mArrBotState[mCurState]->CanStop())
		{
			mArrBotState[mNextState]->Start();
			mCurState = mNextState;
		}
	}

	mTransform->CalculateWorldMatrix();
}

void Bot::LateUpdate(const _double timeDelta)
{
	mTimeDelta = timeDelta;
	mArrBotState[mCurState]->LateUpdate(timeDelta);
}

void Bot::Render()
{
	mDynamicMesh->PlayAnimation(mTimeDelta);

	PipeLine* pipeLine = PipeLine::GetInstance();
	const _matrix matVP = pipeLine->GetTransform(D3DTS_VIEW) * pipeLine->GetTransform(D3DTS_PROJECTION);
	mShader->Get_EffectHandle()->SetMatrix("gMatWorld", &mTransform->GetWorldMatrix());
	mShader->Get_EffectHandle()->SetMatrix("gMatVP", &matVP);
	
	for (_int i = 0; i < mDynamicMesh->GetMeshContinerSize(); ++i)
		mDynamicMesh->Render(mShader, i);
}

void Bot::SetNextState(const BotState nextState)
{
	mNextState = nextState;
}

void Bot::Move(const _vec3 & moveDir)
{
	World_Object* world = GetWorld();
	NaviMesh_Object* naviMesh = world->GetNaviMeshObject();

	_int nextCellIndex = 0;
	_vec3 nextPosition;
	if (naviMesh->Move(mCellIndex, mTransform->GetPosition(), moveDir, nextCellIndex, nextPosition))
	{
		mCellIndex = nextCellIndex;
		mTransform->SetPosition(nextPosition);
	}

	mTransform->CalculateWorldMatrix();
}

void Bot::SetUpAnimation(const _uint index)
{
	mDynamicMesh->SetUpAnimation(index);
}

_bool Bot::Initialize(const Bot::Data & data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("DynamicMesh_Bot", "DynamicMesh_Bot", (Component_Object**)&mDynamicMesh);
	GameObject::AddComponent("Shader_HardwareSkinning", "Shader_HardwareSkinning", (Component_Object**)&mShader);

	mArrBotState.fill(nullptr);
	mCurState = BotState::IDLE;
	mNextState = BotState::IDLE;

	mArrBotState[BotState::IDLE] = BotIdle::Create(this);
	mArrBotState[BotState::RUN] = BotRun::Create(this);

	return true;
}

void Bot::Free()
{
	for (State<Bot>*& state : mArrBotState)
		SafeRelease(state);
	mArrBotState.fill(nullptr);

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
