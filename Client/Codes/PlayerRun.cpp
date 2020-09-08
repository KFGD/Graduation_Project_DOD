#include "stdafx.h"
#include "PlayerRun.h"

#include "Player.h"
#include "KeyManager.h"

PlayerRun::PlayerRun(Player * executer)
	: State<Player>(executer)
	, mKeyManager(KeyManager::GetInstance())
{
}

PlayerRun::~PlayerRun()
{
}

void PlayerRun::Start()
{
	Player* player = GetExecuter();
	player->SetUpAnimation(0);
}

void PlayerRun::Update(const _double timeDelta)
{
	Player* player = GetExecuter();
	if (!InputMove())
		player->SetNextState(Player::PlayerState::IDLE);
	else
	{
		constexpr _float speed = 3.f;
		_vec3 moveVector = mDir * speed * (_float)timeDelta;
		player->Move(moveVector);
	}
}

void PlayerRun::LateUpdate(const _double timeDelta)
{
}

bool PlayerRun::CanStop() const
{
	return true;
}

_bool PlayerRun::InputMove()
{
	_vec3 dir;
	::ZeroMemory(&dir, sizeof(_vec3));
	if (mKeyManager->KeyPresseing(KeyManager::KEY_W))
		dir.z += 1.f;
	if (mKeyManager->KeyPresseing(KeyManager::KEY_S))
		dir.z -= 1.f;
	if (mKeyManager->KeyPresseing(KeyManager::KEY_A))
		dir.x -= 1.f;
	if (mKeyManager->KeyPresseing(KeyManager::KEY_D))
		dir.x += 1.f;

	if (0 == D3DXVec3Length(&dir))
		return false;

	D3DXVec3Normalize(&mDir, &dir);
	return true;
}

_bool PlayerRun::Initialize()
{
	return true;
}

PlayerRun * PlayerRun::Create(Player * executer)
{
	PlayerRun*	pInstnace = new PlayerRun(executer);

	if (false == pInstnace->Initialize())
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create * PlayerRun Instance");
	}

	return pInstnace;
}

void PlayerRun::Free()
{
	State<Player>::Free();
}
