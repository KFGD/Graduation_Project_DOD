#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class GameMode :
	public Base, public IMode
{
	typedef Game::ObjectType Type;

private:
	explicit GameMode();
	virtual ~GameMode() = default;

public:
	virtual void	Active(IWorldController* worldController)	override;
	virtual void	InActive()	override;
	virtual void	Update()	override;

private:
	vector<KObject*>	mObjectList;



};

