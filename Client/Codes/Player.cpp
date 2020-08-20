#include "stdafx.h"
#include "Player.h"

#include "Transform_Object.h"
#include "DynamicMesh_Object.h"

#include "World.h"
#include "PipeLine.h"
#include "Shader.h"

Player::Player()
{
}

void Player::Update(const _double timeDelta)
{
	mTransform->CalculateWorldMatrix();
}

void Player::LateUpdate(const _double timeDelta)
{
	mTimeDelta = timeDelta;
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

	LPDIRECT3DDEVICE9 graphicDevice = GetWorld()->GetGraphicDevice();

	graphicDevice->SetTransform(D3DTS_VIEW, &PipeLine::GetInstance()->GetTransform(D3DTS_VIEW));
	graphicDevice->SetTransform(D3DTS_PROJECTION, &PipeLine::GetInstance()->GetTransform(D3DTS_PROJECTION));
	graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	for (_int i = 0; i < mDynamicMesh->GetMeshContinerSize(); ++i)
		mDynamicMesh->GetMeshContainer(i)->pSoftwareMesh->DrawSubset(0);


	graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	graphicDevice->SetTransform(D3DTS_VIEW, &IDENTITY_MATRIX);
	graphicDevice->SetTransform(D3DTS_PROJECTION, &IDENTITY_MATRIX);
}

_bool Player::Initialize(const Player::Data& data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("DynamicMesh_Player", "DynamicMesh_Player", (Component_Object**)&mDynamicMesh);	
	GameObject::AddComponent("Shader_HardwareSkinning", "Shader_HardwareSkinning", (Component_Object**)&mShader);
	mDynamicMesh->SetUpAnimation(1);

	return true;
}

void Player::Free()
{
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
