#include "stdafx.h"
#include "BotRun.h"

#include "Bot.h"

BotRun::BotRun(Bot* executer)
	: State<Bot>(executer)
{
}

void BotRun::Start()
{
	Bot* bot = GetExecuter();
	bot->SetUpAnimation(0);
}

void BotRun::Update(const _double timeDelta)
{
}

void BotRun::LateUpdate(const _double timeDelta)
{
}

bool BotRun::CanStop() const
{
	return false;
}

_bool BotRun::Initialize()
{
	return true;
}

BotRun * BotRun::Create(Bot * executer)
{
	BotRun*	pInstnace = new BotRun(executer);

	if (false == pInstnace->Initialize())
	{
		SafeRelease(pInstnace);
		MSG_BOX("Failed To Create BotRun Instance");
	}

	return pInstnace;
}

void BotRun::Free()
{
	State<Bot>::Free();
}
