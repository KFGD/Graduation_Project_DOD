#include "stdafx.h"
#include "Player.h"

#include "PlayerIdle.h"
#include "PlayerRun.h"

#include "Transform_Object.h"
#include "DynamicMeshRenderer_Object.h"
#include "World_Object.h"
#include "PipeLine.h"
#include "Shader.h"
#include "NaviMesh_Object.h"
#include "KeyManager.h"

Player::Player()
{
}

void Player::SetUp(World_Object * world)
{
	GameObject::SetUp(world);

	NaviMesh_Object* naviMesh = world->GetNaviMeshObject();
	mCellIndex = naviMesh->FindCellIndex(mTransform->GetPosition());
}

void Player::Update(const _double timeDelta)
{
	if (mCurState != mNextState)
	{
		if (mArrPlayerState[mCurState]->CanStop())
		{
			mArrPlayerState[mNextState]->Start();
			mCurState = mNextState;
		}
	}

	mArrPlayerState[mCurState]->Update(timeDelta);
}

void Player::LateUpdate(const _double timeDelta)
{
	mTimeDelta = timeDelta;
	mArrPlayerState[mCurState]->LateUpdate(timeDelta);
}

void Player::Render()
{
	mDynamicMesh->PlayAnimation(mTimeDelta);

	PipeLine* pipeLine = PipeLine::GetInstance();
	const _matrix matVP = pipeLine->GetTransform(D3DTS_VIEW) * pipeLine->GetTransform(D3DTS_PROJECTION);
	mShader->Get_EffectHandle()->SetMatrix("gMatWorld", &mTransform->GetWorldMatrix());
	mShader->Get_EffectHandle()->SetMatrix("gMatVP", &matVP);

	for (_int i = 0; i < mDynamicMesh->GetMeshContinerSize(); ++i)
		mDynamicMesh->Render(mShader, i);
}

const _vec3 & Player::GetPosition() const
{
	return mTransform->GetPosition();
}

void Player::SetNextState(const PlayerState nextState)
{
	mNextState = nextState;
}

void Player::Move(const _vec3 & moveDir)
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

void Player::SetRotateY(const _float degree)
{
	mTransform->SetRotationY(degree);
}

void Player::SetUpAnimation(const _uint index)
{
	mDynamicMesh->SetUpAnimation(index);
}

_bool Player::Initialize(const Player::Data& data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("DynamicMesh_Player", "DynamicMesh_Player", (Component_Object**)&mDynamicMesh);	
	GameObject::AddComponent("Shader_HardwareSkinning", "Shader_HardwareSkinning", (Component_Object**)&mShader);

	mArrPlayerState.fill(nullptr);
	mCurState = PlayerState::IDLE;
	mNextState = PlayerState::IDLE;

	mArrPlayerState[PlayerState::IDLE] = PlayerIdle::Create(this);
	mArrPlayerState[PlayerState::RUN] = PlayerRun::Create(this);

	return true;
}

void Player::Free()
{
	for (State<Player>*& state : mArrPlayerState)
		SafeRelease(state);
	mArrPlayerState.fill(nullptr);

	GameObject::Free();
}

Player * Player::Create(const Player::Data & data)
{
	Player*	pInstnace = new Player();

	if (false == pInstnace->Initialize(data))
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create Player Instance");
	}

	return pInstnace;
}
