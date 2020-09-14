#pragma once
#include "State.h"

class Bot;
class KeyManager;

class BotIdle :
	public State<Bot>
{
private:
	explicit BotIdle(Bot* executer);
	virtual ~BotIdle() = default;

public:
	virtual void Start() override;
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;
	virtual bool CanStop() const override;

private:
	_bool	Initialize();
	
public:
	static BotIdle*	Create(Bot* executer);
	virtual void	Free() override;
};

