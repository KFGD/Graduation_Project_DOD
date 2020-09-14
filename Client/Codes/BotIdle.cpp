#include "stdafx.h"
#include "BotIdle.h"

#include "Bot.h"

BotIdle::BotIdle(Bot * executer)
	: State<Bot>(executer)
{
}

void BotIdle::Start()
{
	Bot* bot = GetExecuter();
	bot->SetUpAnimation(1);
}

void BotIdle::Update(const _double timeDelta)
{
}

void BotIdle::LateUpdate(const _double timeDelta)
{
}

bool BotIdle::CanStop() const
{
	return true;
}

_bool BotIdle::Initialize()
{
	return true;
}

BotIdle * BotIdle::Create(Bot * executer)
{
	BotIdle*	pInstnace = new BotIdle(executer);

	if (false == pInstnace->Initialize())
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create BotIdle Instance");
	}

	return pInstnace;
}

void BotIdle::Free()
{
	State<Bot>::Free();
}
