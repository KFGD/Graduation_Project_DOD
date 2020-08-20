#pragma once

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class IWorldController
{
protected:
	explicit	IWorldController() = default;
	virtual		~IWorldController() = default;

public:
	virtual _bool	ChangeWorld(const Game::WorldType worldType) = 0;
	virtual _bool	SetUpObjectList(const vector<KObject*>& objectList) = 0;
	virtual _bool	ClearObjectList() = 0;
};