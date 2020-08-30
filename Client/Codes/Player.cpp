#include "stdafx.h"
#include "Player.h"

#include "Transform_Object.h"
#include "DynamicMesh_Object.h"

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
	KeyManager* keyManager = KeyManager::GetInstance();
	_vec3 dir;
	::ZeroMemory(&dir, sizeof(_vec3));
	if (keyManager->KeyPresseing(KeyManager::KEY_W))
		dir.z += 1.f;
	if (keyManager->KeyPresseing(KeyManager::KEY_S))
		dir.z -= 1.f;
	if (keyManager->KeyPresseing(KeyManager::KEY_A))
		dir.x -= 1.f;
	if (keyManager->KeyPresseing(KeyManager::KEY_D))
		dir.x += 1.f;
	
	D3DXVec3Normalize(&dir, &dir);

	constexpr _float speed = 10.f;
	_vec3 newPosition = mTransform->GetPosition() + dir * speed * timeDelta;

	World_Object* world = GetWorld();
	NaviMesh_Object* naviMesh = world->GetNaviMeshObject();

	_int nextCellIndex = 0;
	_vec3 fixPosition;
	if (naviMesh->Move(mCellIndex, newPosition, nextCellIndex, fixPosition))
	{
		mCellIndex = nextCellIndex;
		mTransform->SetPosition(fixPosition);
	}

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

	//LPDIRECT3DDEVICE9 graphicDevice = GetWorld()->GetGraphicDevice();

	//graphicDevice->SetTransform(D3DTS_VIEW, &PipeLine::GetInstance()->GetTransform(D3DTS_VIEW));
	//graphicDevice->SetTransform(D3DTS_PROJECTION, &PipeLine::GetInstance()->GetTransform(D3DTS_PROJECTION));
	//graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	//for (_int i = 0; i < mDynamicMesh->GetMeshContinerSize(); ++i)
	//	mDynamicMesh->GetMeshContainer(i)->pSoftwareMesh->DrawSubset(0);


	//graphicDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	//graphicDevice->SetTransform(D3DTS_VIEW, &IDENTITY_MATRIX);
	//graphicDevice->SetTransform(D3DTS_PROJECTION, &IDENTITY_MATRIX);
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
