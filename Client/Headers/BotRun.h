#pragma once
#include "State.h"

class Bot;

class BotRun :
	public State<Bot>
{
private:
	explicit BotRun(Bot* executer);
	virtual ~BotRun() = default;

public:
	virtual void Start() override;
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;
	virtual bool CanStop() const override;

private:
	_bool	Initialize();

public:
	static	BotRun*	Create(Bot* executer);
	virtual void	Free() override;

};

