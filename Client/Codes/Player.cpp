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
	mDynamicMesh->PlayAnimation(timeDelta);
}

void Player::Render()
{
}

_bool Player::Initialize(const Player::Data& data)
{
	GameObject::AddComponent("Transform", "Transform", (Component_Object**)&mTransform, &Transform_Object::Data(data.Scale, data.Rotation, data.Position));
	GameObject::AddComponent("DynamicMesh_Bot", "DynamicMesh_Bot", (Component_Object**)&mDynamicMesh);	GameObject::AddComponent("Shader_HardwareSkinning", "Shader_HardwareSkinning", (Component_Object**)&mShader);
	GameObject::AddComponent("Shader_HardwareSkinning", "Shader_HardwareSkinning", (Component_Object**)&mShader);

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
