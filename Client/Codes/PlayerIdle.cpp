#include "stdafx.h"
#include "PlayerIdle.h"

#include "KeyManager.h"
#include "Player.h"

PlayerIdle::PlayerIdle(Player * executer)
	: State<Player>(executer)
	, mKeyManager(KeyManager::GetInstance())
{
}

void PlayerIdle::Start()
{
	Player* player = GetExecuter();
	player->SetUpAnimation(1);
}

void PlayerIdle::Update(const _double timeDelta)
{
	Player* player = GetExecuter();

	if (InputMove())
		player->SetNextState(Player::PlayerState::RUN);
}

void PlayerIdle::LateUpdate(const _double timeDelta)
{
}

bool PlayerIdle::CanStop() const
{
	return true;
}

_bool PlayerIdle::Initialize()
{
	return true;
}

_bool PlayerIdle::InputMove()
{
	constexpr array<_ulong, 4> arrayMoveKey = { KeyManager::KEY_W, KeyManager::KEY_A, KeyManager::KEY_S, KeyManager::KEY_D };
	for (_size_t i = 0; i < 4; ++i)
	{
		if (mKeyManager->KeyDown(arrayMoveKey[i]))
			return true;
	}

	return false;
}

PlayerIdle * PlayerIdle::Create(Player * executer)
{
	PlayerIdle*	pInstnace = new PlayerIdle(executer);

	if (false == pInstnace->Initialize())
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create PlayerIdle Instance");
	}

	return pInstnace;
}

void PlayerIdle::Free()
{
	State<Player>::Free();
}
