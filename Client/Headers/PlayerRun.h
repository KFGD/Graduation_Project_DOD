#pragma once
#include "State.h"
#include "Base.h"

class Player;
class KeyManager;

class PlayerRun :
	public State<Player>
{
public:
	explicit PlayerRun(Player* executer);
	virtual ~PlayerRun();

public:
	virtual void Start() override;
	virtual void Update(const _double timeDelta) override;
	virtual void LateUpdate(const _double timeDelta) override;
	virtual bool CanStop() const override;

private:
	_bool	InputMove();

private:
	_bool	Initialize();

private:
	KeyManager*	mKeyManager = nullptr;
	_vec3		mDir;

public:
	static PlayerRun*	Create(Player* executer);
	virtual void Free() override;
};

