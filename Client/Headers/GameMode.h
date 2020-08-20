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
	virtual void	InActive(IWorldController* worldController)	override;
	virtual void	Update(IWorldController* worldController)	override;

private:
	vector<KObject*>	mObjectList;



};

