#pragma once
#include "Base.h"
#include "Mode.h"

#include "Defines.h"
#include "Client_Defines.h"

class KObject;

class GameMode :
	public Mode
{
	typedef Game::ObjectType Type;

private:
	explicit GameMode();
	virtual ~GameMode() = default;

public:
	virtual void	Active(IWorldController* worldController)	override;
	virtual void	InActive(IWorldController* worldController)	override;
	virtual void	Update(IWorldController* worldController)	override;
	virtual void	Render(LPDIRECT3DDEVICE9 graphicDevice)		override;

private:
	_bool	Initialize(LPDIRECT3DDEVICE9 graphicDevice);
	void	ClearObjectList();

private:
	vector<KObject*>	mObjectList;


public:
	static GameMode*	Create(LPDIRECT3DDEVICE9 graphicDevice);
	virtual void	Free() override;

};

