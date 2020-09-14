#pragma once
#include "State.h"
#include "Base.h"

class Player;
class KeyManager;

class PlayerIdle :
	public State<Player>
{
private:
	explicit PlayerIdle(Player* executer);
	virtual ~PlayerIdle() = default;

public:
	virtual void Start() override;
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;
	virtual bool CanStop() const override;
	
private:
	_bool	Initialize();
	_bool	InputMove();

private:
	KeyManager*	mKeyManager = nullptr;

public:
	static	PlayerIdle*		Create(Player* executer);
	virtual void Free() override;
};

